#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>
#include <string_view>
#include <type_traits>
#include <utility>

#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "InventoryChanger.h"
#include "../Texture.h"

#include "../nlohmann/json.hpp"

#include "../SDK/ClassId.h"
#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/FileSystem.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/Panorama.h"
#include "../SDK/PlayerResource.h"
#include "../SDK/Platform.h"
#include "../SDK/WeaponId.h"

#include "../Helpers.h"

#include "StaticData.h"

constexpr auto isKnife(WeaponId id) noexcept
{
    return (id >= WeaponId::Bayonet && id <= WeaponId::SkeletonKnife) || id == WeaponId::KnifeT || id == WeaponId::Knife;
}

struct StickerConfig {
    int stickerID = 0;
    float wear = 0.0f;
};

struct DynamicSkinData {
    bool isSouvenir = false;
    float wear = 0.0f;
    int seed = 1;
    int statTrak = -1;
    std::array<StickerConfig, 5> stickers;
    std::string nameTag;
};

struct PatchConfig {
    int patchID = 0;
};

struct DynamicAgentData {
    std::array<PatchConfig, 5> patches;
};

struct DynamicGloveData {
    float wear = 0.0f;
    int seed = 1;
};

struct DynamicMusicData {
    int statTrak = -1;
};

static std::vector<DynamicSkinData> dynamicSkinData;
static std::vector<DynamicGloveData> dynamicGloveData;
static std::vector<DynamicAgentData> dynamicAgentData;
static std::vector<DynamicMusicData> dynamicMusicData;

constexpr auto BASE_ITEMID = 1152921504606746975;

static float randomFloat(float min, float max) noexcept
{
    std::mt19937 gen{ std::random_device{}() };
    std::uniform_real_distribution dis{ min, max };
    return dis(gen);
}

static int randomInt(int min, int max) noexcept
{
    std::mt19937 gen{ std::random_device{}() };
    std::uniform_int_distribution dis{ min, max };
    return dis(gen);
}

struct InventoryItem {
private:
    std::size_t itemIndex;
    std::size_t dynamicDataIndex = static_cast<std::size_t>(-1);

    // TODO: This should be moved to where items are generated from cases
    static float generateWear(bool factoryNewOnly) noexcept
    {
        if (factoryNewOnly)
            return randomFloat(0.0f, 0.07f);

        float wear;
        if (const auto condition = randomInt(1, 10000); condition <= 1471)
            wear = randomFloat(0.0f, 0.07f);
        else if (condition <= 3939)
            wear = randomFloat(0.07f, 0.15f);
        else if (condition <= 8257)
            wear = randomFloat(0.15f, 0.38f);
        else if (condition <= 9049)
            wear = randomFloat(0.38f, 0.45f);
        else
            wear = randomFloat(0.45f, 1.0f);
        return wear;
    }
public:
    explicit InventoryItem(std::size_t itemIndex, std::size_t dynamicDataIndex) noexcept : itemIndex{ itemIndex }, dynamicDataIndex{ dynamicDataIndex } {}

    void markAsDeleted() noexcept { itemIndex = static_cast<std::size_t>(-1); }
    bool isDeleted() const noexcept { return itemIndex == static_cast<std::size_t>(-1); }
    void markToDelete() noexcept { itemIndex = static_cast<std::size_t>(-2); }
    bool shouldDelete() const noexcept { return itemIndex == static_cast<std::size_t>(-2); }

    bool isSticker() const noexcept { return !isDeleted() && !shouldDelete() && get().isSticker(); }
    bool isSkin() const noexcept { return !isDeleted() && !shouldDelete() && get().isSkin(); }
    bool isGlove() const noexcept { return !isDeleted() && !shouldDelete() && get().isGlove(); }
    bool isMusic() const noexcept { return !isDeleted() && !shouldDelete() && get().isMusic(); }
    bool isAgent() const noexcept { return !isDeleted() && !shouldDelete() && get().isAgent(); }
    bool isCollectible() const noexcept { return !isDeleted() && !shouldDelete() && get().isCollectible(); }
    bool isCase() const noexcept { return !isDeleted() && !shouldDelete() && get().isCase(); }
    bool isCaseKey() const noexcept { return !isDeleted() && !shouldDelete() && get().isCaseKey(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(dynamicDataIndex != static_cast<std::size_t>(-1)); return dynamicDataIndex; }

    const StaticData::GameItem& get() const noexcept { assert(!isDeleted() && !shouldDelete()); return StaticData::gameItems()[itemIndex]; }
};

static std::vector<InventoryItem> inventory;

struct ToEquip {
    ToEquip(Team team, int slot, std::size_t index) : team{ team }, slot{ slot }, index{ index } {}

    Team team;
    int slot;
    std::size_t index;
};

static std::vector<ToEquip> toEquip;

class Inventory {
public:
    static constexpr auto INVALID_DYNAMIC_DATA_IDX = static_cast<std::size_t>(-1);

    static void addItem(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        instance().toAdd.emplace_back(gameItemIndex, dynamicDataIdx, asUnacknowledged);
    }

    static std::uint64_t addItemNow(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        return instance()._addItem(gameItemIndex, dynamicDataIdx, asUnacknowledged);
    }

    static void deleteItemNow(std::uint64_t itemID) noexcept
    {
        instance()._deleteItem(itemID);
    }

    static void runFrame() noexcept
    {
        instance()._runFrame();
    }

    static InventoryItem* getItem(std::uint64_t itemID) noexcept
    {
        return instance()._getItem(itemID);
    }

    static std::uint64_t recreateItem(std::uint64_t itemID) noexcept
    {
        return instance()._recreateItem(itemID);
    }

    static void clear() noexcept
    {
        instance()._clear();
    }
private:
    InventoryItem* _getItem(std::uint64_t itemID) noexcept
    {
        if (itemID >= BASE_ITEMID && static_cast<std::size_t>(itemID - BASE_ITEMID) < inventory.size())
            return &inventory[static_cast<std::size_t>(itemID - BASE_ITEMID)];
        return nullptr;
    }

    std::uint64_t _createSOCItem(const InventoryItem& inventoryItem, bool asUnacknowledged) noexcept
    {
        const auto localInventory = memory->inventoryManager->getLocalInventory();
        if (!localInventory)
            return 0;

        const auto baseTypeCache = localInventory->getItemBaseTypeCache();
        if (!baseTypeCache)
            return 0;

        static const auto baseInvID = localInventory->getHighestIDs().second;

        const auto econItem = memory->createEconItemSharedObject();
        econItem->itemID = BASE_ITEMID + inventory.size() - 1;
        econItem->originalID = 0;
        econItem->accountID = localInventory->getAccountID();
        econItem->inventory = asUnacknowledged ? 0 : baseInvID + inventory.size();

        const auto& item = inventoryItem.get();
        econItem->rarity = item.rarity;
        econItem->quality = 4;
        econItem->weaponId = item.weaponID;

        if (item.isSticker() || item.isPatch() || item.isGraffiti() || item.isSealedGraffiti()) {
            econItem->setStickerID(0, StaticData::paintKits()[item.dataIndex].id);
        } else if (item.isMusic()) {
            econItem->setMusicID(StaticData::paintKits()[item.dataIndex].id);
            const auto& dynamicData = dynamicMusicData[inventoryItem.getDynamicDataIndex()];
            if (dynamicData.statTrak > -1) {
                econItem->setStatTrak(dynamicData.statTrak);
                econItem->setStatTrakType(1);
                econItem->quality = 9;
            }
        } else if (item.isSkin()) {
            econItem->setPaintKit(static_cast<float>(StaticData::paintKits()[item.dataIndex].id));

            const auto& dynamicData = dynamicSkinData[inventoryItem.getDynamicDataIndex()];
            if (dynamicData.isSouvenir) {
                econItem->quality = 12;
            } else {
                if (dynamicData.statTrak > -1) {
                    econItem->setStatTrak(dynamicData.statTrak);
                    econItem->setStatTrakType(0);
                    econItem->quality = 9;
                }
                if (isKnife(econItem->weaponId))
                    econItem->quality = 3;
            }

            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
            memory->setCustomName(econItem, dynamicData.nameTag.c_str());

            for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
                const auto& sticker = dynamicData.stickers[j];
                if (sticker.stickerID == 0)
                    continue;

                econItem->setStickerID(j, sticker.stickerID);
                econItem->setStickerWear(j, sticker.wear);
            }
        } else if (item.isGlove()) {
            econItem->quality = 3;
            econItem->setPaintKit(static_cast<float>(StaticData::paintKits()[item.dataIndex].id));

            const auto& dynamicData = dynamicGloveData[inventoryItem.getDynamicDataIndex()];
            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
        } else if (item.isCollectible()) {
            if (StaticData::collectibles()[item.dataIndex].isOriginal)
                econItem->quality = 1;
        } else if (item.isAgent()) {
            const auto& dynamicData = dynamicAgentData[inventoryItem.getDynamicDataIndex()];
            for (std::size_t j = 0; j < dynamicData.patches.size(); ++j) {
                const auto& patch = dynamicData.patches[j];
                if (patch.patchID == 0)
                    continue;

                econItem->setStickerID(j, patch.patchID);
            }
        }

        baseTypeCache->addObject(econItem);
        memory->addEconItem(localInventory, econItem, false, false, false);

        if (const auto inventoryComponent = *memory->uiComponentInventory) {
            memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "recent", "0");
            memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "updated", "0");
        }

        if (const auto view = memory->findOrCreateEconItemViewForItemID(econItem->itemID))
            memory->clearInventoryImageRGBA(view);

        return econItem->itemID;
    }

    void _deleteItem(std::uint64_t itemID) noexcept
    {
        const auto item = _getItem(itemID);
        if (!item)
            return;

        const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
        if (!view)
            return;

        const auto econItem = memory->getSOCData(view);
        if (!econItem)
            return;

        const auto localInventory = memory->inventoryManager->getLocalInventory();
        if (!localInventory)
            return;

        localInventory->soDestroyed(localInventory->getSOID(), (SharedObject*)econItem, 4);

        if (const auto baseTypeCache = localInventory->getItemBaseTypeCache())
            baseTypeCache->removeObject(econItem);

        item->markAsDeleted();
    }

    static std::size_t createDefaultDynamicData(std::size_t gameItemIndex) noexcept
    {
        std::size_t index = static_cast<std::size_t>(-1);

        if (const auto& item = StaticData::gameItems()[gameItemIndex]; item.isSkin()) {
            const auto& staticData = StaticData::paintKits()[item.dataIndex];
            DynamicSkinData dynamicData;
            dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, randomFloat(0.0f, 0.07f));
            dynamicData.seed = randomInt(1, 1000);
            dynamicSkinData.push_back(dynamicData);
            index = dynamicSkinData.size() - 1;
        } else if (item.isGlove()) {
            const auto& staticData = StaticData::paintKits()[item.dataIndex];
            DynamicGloveData dynamicData;
            dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, randomFloat(0.0f, 0.07f));
            dynamicData.seed = randomInt(1, 1000);
            dynamicGloveData.push_back(dynamicData);
            index = dynamicGloveData.size() - 1;
        } else if (item.isAgent()) {
            dynamicAgentData.push_back({});
            index = dynamicAgentData.size() - 1;
        } else if (item.isMusic()) {
            dynamicMusicData.push_back({});
            index = dynamicMusicData.size() - 1;
        }

        return index;
    }

    std::uint64_t _addItem(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        return _createSOCItem(inventory.emplace_back(gameItemIndex, dynamicDataIdx != INVALID_DYNAMIC_DATA_IDX ? dynamicDataIdx : createDefaultDynamicData(gameItemIndex)), asUnacknowledged);
    }

    std::uint64_t _recreateItem(std::uint64_t itemID) noexcept
    {
        const auto item = _getItem(itemID);
        if (!item)
            return 0;

        auto itemCopy = *item;

        if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
            if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID)) {
                if (const auto econItem = memory->getSOCData(view)) {
                    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(econItem->weaponId)) {
                        if (const auto slotCT = def->getLoadoutSlot(Team::CT); localInventory->getItemInLoadout(Team::CT, slotCT) == view)
                            toEquip.emplace_back(Team::CT, slotCT, inventory.size());
                        if (const auto slotTT = def->getLoadoutSlot(Team::TT); localInventory->getItemInLoadout(Team::TT, slotTT) == view)
                            toEquip.emplace_back(Team::TT, slotTT, inventory.size());
                    }
                }
            }
        }

        _deleteItem(itemID);
        _createSOCItem(inventory.emplace_back(std::move(itemCopy)), false);
    }
    
    void _addItems() noexcept
    {
        for (const auto [index, dynamicDataIndex, asUnacknowledged] : toAdd)
            _addItem(index, dynamicDataIndex, asUnacknowledged);
        toAdd.clear();
    }

    void _clear() noexcept
    {
        for (std::size_t i = 0; i < inventory.size(); ++i)
            _deleteItem(BASE_ITEMID + i);
        inventory.clear();
    }

    void _runFrame() noexcept
    {
        _addItems();
    }

    static Inventory& instance() noexcept
    {
        static Inventory inventory;
        return inventory;
    }

    std::vector<std::tuple<std::size_t, std::size_t, bool>> toAdd;
};

static void addToInventory(const std::unordered_map<std::size_t, int>& toAdd) noexcept
{
    for (const auto [idx, count] : toAdd) {
        for (int i = 0; i < count; ++i)
            Inventory::addItem(idx, Inventory::INVALID_DYNAMIC_DATA_IDX, true);
    }
}

static void sendInventoryUpdatedEvent() noexcept
{
    if (const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_MyPersona_InventoryUpdated")); idx != -1) {
        if (const auto eventPtr = memory->registeredPanoramaEvents->memory[idx].value.makeEvent(nullptr))
            interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(eventPtr);
    }
}

static Entity* make_glove(int entry, int serial) noexcept
{
    static std::add_pointer_t<Entity* __CDECL(int, int)> createWearable = nullptr;

    if (!createWearable) {
        createWearable = []() -> decltype(createWearable) {
            for (auto clientClass = interfaces->client->getAllClasses(); clientClass; clientClass = clientClass->next)
                if (clientClass->classId == ClassId::EconWearable)
                    return clientClass->createFunction;
            return nullptr;
        }();
    }

    if (!createWearable)
        return nullptr;

    createWearable(entry, serial);
    return interfaces->entityList->getEntity(entry);
}

static void applyGloves(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto itemView = localInventory.getItemInLoadout(local->getTeamNumber(), 41);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isGlove())
        return;

    const auto& itemData = StaticData::paintKits()[item->get().dataIndex];

    const auto wearables = local->wearables();
    static int gloveHandle = 0;

    auto glove = interfaces->entityList->getEntityFromHandle(wearables[0]);
    if (!glove)
        glove = interfaces->entityList->getEntityFromHandle(gloveHandle);
    if (!glove) {
        const auto entry = interfaces->entityList->getHighestEntityIndex() + 1;
        const auto serial = randomInt(0, 0x1000);
        glove = make_glove(entry, serial);
    }

    if (!glove)
        return;

    wearables[0] = gloveHandle = glove->handle();
    glove->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
    glove->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
    glove->entityQuality() = 3;
    glove->initialized() = true;
    memory->equipWearable(glove, local);
    local->body() = 1;

    const auto attributeList = glove->econItemView().getAttributeList();
    const auto& dynamicData = dynamicGloveData[item->getDynamicDataIndex()];
    memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.id));
    memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", dynamicData.wear);
    memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(dynamicData.seed));

    if (auto& definitionIndex = glove->itemDefinitionIndex2(); definitionIndex != item->get().weaponID) {
        definitionIndex = item->get().weaponID;

        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->get().weaponID)) {
            glove->setModelIndex(interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel()));
            glove->preDataUpdate(0);
        }
    }
}

static void applyKnife(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto localXuid = local->getSteamId();

    const auto itemView = localInventory.getItemInLoadout(local->getTeamNumber(), 0);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isSkin())
        return;

    auto& weapons = local->weapons();

    for (auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;

        auto& definitionIndex = weapon->itemDefinitionIndex2();
        if (!isKnife(definitionIndex))
            continue;

        if (weapon->originalOwnerXuid() != localXuid)
            continue;

        weapon->accountID() = localInventory.getAccountID();
        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
        weapon->entityQuality() = 3;

        if (definitionIndex != item->get().weaponID) {
            definitionIndex = item->get().weaponID;

            if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->get().weaponID)) {
                weapon->setModelIndex(interfaces->modelInfo->getModelIndex(def->getPlayerDisplayModel()));
                weapon->preDataUpdate(0);
            }
        }
    }

    const auto viewModel = interfaces->entityList->getEntityFromHandle(local->viewModel());
    if (!viewModel)
        return;

    const auto viewModelWeapon = interfaces->entityList->getEntityFromHandle(viewModel->weapon());
    if (!viewModelWeapon)
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(viewModelWeapon->itemDefinitionIndex2());
    if (!def)
        return;

    viewModel->modelIndex() = interfaces->modelInfo->getModelIndex(def->getPlayerDisplayModel());

    const auto worldModel = interfaces->entityList->getEntityFromHandle(viewModelWeapon->weaponWorldModel());
    if (!worldModel)
        return;

    worldModel->modelIndex() = interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel());
}

static void applyWeapons(CSPlayerInventory& localInventory, Entity* local) noexcept
{
    const auto localTeam = local->getTeamNumber();
    const auto localXuid = local->getSteamId();

    auto& weapons = local->weapons();
    for (auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;

        const auto& definitionIndex = weapon->itemDefinitionIndex2();
        if (isKnife(definitionIndex))
            continue;

        if (weapon->originalOwnerXuid() != localXuid)
            continue;

        const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(definitionIndex);
        if (!def)
            continue;

        const auto loadoutSlot = def->getLoadoutSlot(localTeam);
        const auto itemView = localInventory.getItemInLoadout(localTeam, loadoutSlot);
        if (!itemView)
            continue;

        const auto soc = memory->getSOCData(itemView);
        if (!soc || !Inventory::getItem(soc->itemID))
            continue;

        weapon->accountID() = localInventory.getAccountID();
        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
    }
}

static void onPostDataUpdateStart(int localHandle) noexcept
{
    const auto local = interfaces->entityList->getEntityFromHandle(localHandle);
    if (!local)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    applyGloves(*localInventory, local);
    applyKnife(*localInventory, local);
    applyWeapons(*localInventory, local);
}

static bool hudUpdateRequired{ false };

static void updateHud() noexcept
{
    if (auto hud_weapons = memory->findHudElement(memory->hud, "CCSGO_HudWeaponSelection") - WIN32_LINUX(0x28, 62)) {
        for (int i = 0; i < *(hud_weapons + WIN32_LINUX(32, 52)); i++)
            i = memory->clearHudWeapon(hud_weapons, i);
    }
    hudUpdateRequired = false;
}

static void initItemCustomizationNotification(const char* typeStr, const char* itemID) noexcept
{
    if (const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_Inventory_ItemCustomizationNotification")); idx != -1) {
        std::string args; args += "0,'"; args += typeStr; args += "','"; args += itemID; args += '\'';
        const char* dummy;
        if (const auto event = memory->registeredPanoramaEvents->memory[idx].value.createEventFromString(nullptr, args.c_str(), &dummy))
            interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(event);
    }
}

static [[deprecated("Use Inventory::deleteItemNow()")]] void removeItemFromInventory(CSPlayerInventory* inventory, SharedObjectTypeCache<EconItem>* cache, EconItem* econItem) noexcept
{
    inventory->soDestroyed(inventory->getSOID(), (SharedObject*)econItem, 4);
    cache->removeObject(econItem);
}

class ToolUser {
public:
    enum class Action {
        Use,
        WearSticker,
        RemoveNameTag
    };

    static void setDestItem(std::uint64_t itemID, Action action) noexcept
    {
        instance().destItemID = itemID;
        instance().action = action;
        instance().useTime = memory->globalVars->realtime + randomFloat(0.18f, 0.48f);
    }

    static void setTool(std::uint64_t itemID) noexcept { instance().toolItemID = itemID; }
    static void setNameTag(const char* nameTag) noexcept { instance().nameTag = nameTag; }
    static void setStickerSlot(int slot) noexcept { instance().stickerSlot = slot; }

    static void preAddItems(CSPlayerInventory& localInventory) noexcept { instance()._preAddItems(localInventory); }
    static void postAddItems() noexcept { instance()._postAddItems(); }
private:
    void _postAddItems() noexcept
    {
        if (recreatedItemID && customizationString && useTime <= memory->globalVars->realtime) {
            initItemCustomizationNotification(customizationString, std::to_string(recreatedItemID).c_str());
            recreatedItemID = 0;
        }
    }

    void _wearSticker() noexcept
    {
        const auto dest = Inventory::getItem(destItemID);
        if (!dest)
            return;

        if (dest->isSkin()) {
            constexpr auto wearStep = 0.12f;
            const auto newWear = (dynamicSkinData[dest->getDynamicDataIndex()].stickers[stickerSlot].wear += wearStep);
            const auto shouldRemove = (newWear >= 1.0f + wearStep);

            if (shouldRemove)
                dynamicSkinData[dest->getDynamicDataIndex()].stickers[stickerSlot] = {};

            if (const auto view = memory->findOrCreateEconItemViewForItemID(destItemID)) {
                if (const auto soc = memory->getSOCData(view)) {
                    if (shouldRemove) {
                        soc->setStickerID(stickerSlot, 0);
                        soc->setStickerWear(stickerSlot, 0.0f);
                    } else {
                        soc->setStickerWear(stickerSlot, newWear);
                    }
                }
            }

            sendInventoryUpdatedEvent();

            if (shouldRemove)
                initItemCustomizationNotification("sticker_remove", std::to_string(destItemID).c_str());

            destItemID = 0;
        } else if (dest->isAgent()) {
            dynamicAgentData[dest->getDynamicDataIndex()].patches[stickerSlot] = {};

            if (const auto view = memory->findOrCreateEconItemViewForItemID(destItemID)) {
                if (const auto soc = memory->getSOCData(view))
                    soc->setStickerID(stickerSlot, 0);
            }

            sendInventoryUpdatedEvent();
            initItemCustomizationNotification("patch_remove", std::to_string(destItemID).c_str());
            destItemID = 0;
        }
    }

    void _removeNameTag(CSPlayerInventory& localInventory) noexcept
    {
        const auto dest = Inventory::getItem(destItemID);
        if (!dest || !dest->isSkin())
            return;

        dynamicSkinData[dest->getDynamicDataIndex()].nameTag.clear();
        Inventory::recreateItem(destItemID);
    }

    void _preAddItems(CSPlayerInventory& localInventory) noexcept
    {
        const auto destItemValid = Inventory::getItem(destItemID) != nullptr;

        if (action == Action::WearSticker && destItemValid && useTime <= memory->globalVars->realtime) {
            _wearSticker();
        } else if (action == Action::RemoveNameTag && destItemValid) {
            _removeNameTag(localInventory);
        } else if (action == Action::Use) {
            if (const auto tool = Inventory::getItem(toolItemID)) {
                const auto& toolItem = tool->get();

                if (toolItem.isSealedGraffiti()) {
                    Inventory::deleteItemNow(toolItemID);

                    const auto it = std::ranges::find_if(StaticData::gameItems(), [graffitiID = StaticData::paintKits()[toolItem.dataIndex].id](const auto& item) { return item.isGraffiti() && StaticData::paintKits()[item.dataIndex].id == graffitiID; });
                    if (it != StaticData::gameItems().end()) {
                        recreatedItemID = BASE_ITEMID + inventory.size();
                        customizationString = "graffity_unseal";
                        Inventory::addItemNow(std::distance(StaticData::gameItems().begin(), it), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
                    }
                } else if (toolItem.isOperationPass()) {
                    tool->markToDelete();

                    const auto coinID = toolItem.weaponID != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(toolItem.weaponID) + 1) : WeaponId::BronzeOperationHydraCoin;
                    const auto it = std::ranges::find(StaticData::gameItems(), coinID, &StaticData::GameItem::weaponID);
                    if (it != StaticData::gameItems().end())
                        Inventory::addItemNow(std::distance(StaticData::gameItems().begin(), it), Inventory::INVALID_DYNAMIC_DATA_IDX, true);
                } else if (destItemValid) {
                    auto& dest = inventory[static_cast<std::size_t>(destItemID - BASE_ITEMID)];
                    if ((dest.isSkin() && (toolItem.isSticker() || toolItem.isNameTag())) || (dest.isAgent() && toolItem.isPatch()) || (dest.isCase() && tool->isCaseKey())) {
                        if (const auto view = memory->findOrCreateEconItemViewForItemID(destItemID)) {
                            const auto isCase = dest.isCase();
                            if (dest.isSkin()) {
                                if (toolItem.isSticker()) {
                                    const auto& sticker = StaticData::paintKits()[toolItem.dataIndex];
                                    dynamicSkinData[dest.getDynamicDataIndex()].stickers[stickerSlot].stickerID = sticker.id;
                                    dynamicSkinData[dest.getDynamicDataIndex()].stickers[stickerSlot].wear = 0.0f;
                                    customizationString = "sticker_apply";
                                } else if (toolItem.isNameTag()) {
                                    dynamicSkinData[dest.getDynamicDataIndex()].nameTag = nameTag;
                                    customizationString = "nametag_add";
                                }
                            } else if (dest.isAgent()) {
                                const auto& patch = StaticData::paintKits()[toolItem.dataIndex];
                                dynamicAgentData[dest.getDynamicDataIndex()].patches[stickerSlot].patchID = patch.id;
                                customizationString = "patch_apply";
                            } else if (isCase) {
                                tool->markToDelete();

                                const auto& caseData = StaticData::cases()[dest.get().dataIndex];
                                assert(caseData.hasLoot());
                                if (caseData.hasLoot()) {
                                    const auto unlockedItemIdx = StaticData::caseLoot()[randomInt(static_cast<int>(caseData.lootBeginIdx), static_cast<int>(caseData.lootEndIdx - 1))];
                                    std::size_t dynamicDataIdx = Inventory::INVALID_DYNAMIC_DATA_IDX;

                                    if (const auto& item = StaticData::gameItems()[unlockedItemIdx]; item.isSkin() && randomInt(0, 9) == 0) {
                                        DynamicSkinData dynamicData;
                                        dynamicData.statTrak = 0;
                                        dynamicSkinData.push_back(std::move(dynamicData));
                                        dynamicDataIdx = dynamicSkinData.size() - 1;
                                    }

                                    Inventory::deleteItemNow(destItemID);
                                    recreatedItemID = Inventory::addItemNow(unlockedItemIdx, dynamicDataIdx, false);
                                    customizationString = "crate_unlock";
                                }
                            }

                            if (!isCase) {
                                Inventory::deleteItemNow(toolItemID);
                                recreatedItemID = Inventory::recreateItem(destItemID);
                            }
                        }
                    }
                }
            } else if (destItemValid) {
                const auto dest = Inventory::getItem(destItemID);
                if (dest && dest->isCase()) {
                    const auto& caseData = StaticData::cases()[dest->get().dataIndex];
                    assert(caseData.hasLoot());
                    if (caseData.hasLoot()) {
                        const auto unlockedItemIdx = StaticData::caseLoot()[randomInt(static_cast<int>(caseData.lootBeginIdx), static_cast<int>(caseData.lootEndIdx - 1))];
                        std::size_t dynamicDataIdx = Inventory::INVALID_DYNAMIC_DATA_IDX;
                        if (const auto& item = StaticData::gameItems()[unlockedItemIdx]; caseData.willProduceStatTrak && item.isMusic()) {
                            DynamicMusicData dynamicData;
                            dynamicData.statTrak = 0;
                            dynamicMusicData.push_back(std::move(dynamicData));
                            dynamicDataIdx = dynamicMusicData.size() - 1;
                        } else if (caseData.isSouvenirPackage && item.isSkin()) {
                            DynamicSkinData dynamicData;
                            dynamicData.isSouvenir = true;
                            dynamicSkinData.push_back(std::move(dynamicData));
                            dynamicDataIdx = dynamicSkinData.size() - 1;
                        }
                        Inventory::deleteItemNow(destItemID);
                        recreatedItemID = Inventory::addItemNow(unlockedItemIdx, dynamicDataIdx, false);
                        customizationString = "crate_unlock";
                    }
                }
            }
            toolItemID = destItemID = 0;
        }
    }

    static ToolUser& instance() noexcept
    {
        static ToolUser toolUser;
        return toolUser;
    }

    std::uint64_t toolItemID = 0;
    std::uint64_t destItemID = 0;
    std::uint64_t recreatedItemID = 0;
    Action action;
    float useTime = 0.0f;
    int stickerSlot = 0;
    const char* customizationString = nullptr;
    std::string nameTag;
};

void InventoryChanger::setToolToUse(std::uint64_t itemID) noexcept { ToolUser::setTool(itemID); }
void InventoryChanger::setItemToApplyTool(std::uint64_t itemID) noexcept { ToolUser::setDestItem(itemID, ToolUser::Action::Use); }
void InventoryChanger::setItemToWearSticker(std::uint64_t itemID) noexcept { ToolUser::setDestItem(itemID, ToolUser::Action::WearSticker); }
void InventoryChanger::setItemToRemoveNameTag(std::uint64_t itemID) noexcept { ToolUser::setDestItem(itemID, ToolUser::Action::RemoveNameTag);}
void InventoryChanger::setStickerApplySlot(int slot) noexcept { ToolUser::setStickerSlot(slot); }
void InventoryChanger::setStickerSlotToWear(int slot) noexcept { ToolUser::setStickerSlot(slot); }
void InventoryChanger::setNameTagString(const char* str) noexcept { ToolUser::setNameTag(str); }

void InventoryChanger::deleteItem(std::uint64_t itemID) noexcept
{
    if (const auto item = Inventory::getItem(itemID))
        item->markToDelete();
}

void InventoryChanger::acknowledgeItem(std::uint64_t itemID) noexcept
{
    if (Inventory::getItem(itemID) == nullptr)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID)) {
        if (const auto soc = memory->getSOCData(view)) {
            soc->inventory = localInventory->getHighestIDs().second;
            localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
        }
    }
}

static void applyMusicKit(CSPlayerInventory& localInventory) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (pr == nullptr)
        return;

    const auto itemView = localInventory.getItemInLoadout(Team::None, 54);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isMusic())
        return;

    const auto& itemData = StaticData::paintKits()[item->get().dataIndex];
    pr->musicID()[localPlayer->index()] = itemData.id;
}

static void applyPlayerAgent(CSPlayerInventory& localInventory) noexcept
{
    if (!localPlayer)
        return;

    const auto itemView = localInventory.getItemInLoadout(localPlayer->getTeamNumber(), 38);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isAgent())
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item->get().weaponID);
    if (!def)
        return;

    const auto model = def->getPlayerDisplayModel();
    if (!model)
        return;

    const auto& dynamicData = dynamicAgentData[item->getDynamicDataIndex()];
    for (std::size_t i = 0; i < dynamicData.patches.size(); ++i) {
        if (const auto& patch = dynamicData.patches[i]; patch.patchID != 0)
            localPlayer->playerPatchIndices()[i] = patch.patchID;
    }

    const auto idx = interfaces->modelInfo->getModelIndex(model);
    localPlayer->setModelIndex(idx);

    if (const auto ragdoll = interfaces->entityList->getEntityFromHandle(localPlayer->ragdoll()))
        ragdoll->setModelIndex(idx);
}

static void applyMedal(CSPlayerInventory& localInventory) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (!pr)
        return;

    const auto itemView = localInventory.getItemInLoadout(Team::None, 55);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isCollectible())
        return;

    pr->activeCoinRank()[localPlayer->index()] = static_cast<int>(item->get().weaponID);
}

void InventoryChanger::run(FrameStage stage) noexcept
{
    static int localPlayerHandle = -1;

    if (localPlayer)
        localPlayerHandle = localPlayer->handle();

    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        onPostDataUpdateStart(localPlayerHandle);
        if (hudUpdateRequired && localPlayer && !localPlayer->isDormant())
            updateHud();
    }

    if (stage != FrameStage::RENDER_START)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto baseTypeCache = localInventory->getItemBaseTypeCache();
    if (!baseTypeCache)
        return;

    applyMusicKit(*localInventory);
    applyPlayerAgent(*localInventory);
    applyMedal(*localInventory);

    static const auto baseInvID = localInventory->getHighestIDs().second;

    ToolUser::preAddItems(*localInventory);

    Inventory::runFrame();

    for (std::size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i].shouldDelete()) {
            if (const auto view = memory->getInventoryItemByItemID(localInventory, BASE_ITEMID + i)) {
                if (const auto econItem = memory->getSOCData(view)) {
                    removeItemFromInventory(localInventory, baseTypeCache, econItem);
                }
            }
            inventory[i].markAsDeleted();
            continue;
        }

        if (inventory[i].isDeleted() || memory->getInventoryItemByItemID(localInventory, BASE_ITEMID + i))
            continue;

        const auto& item = inventory[i].get();

        const auto econItem = memory->createEconItemSharedObject();
        econItem->itemID = BASE_ITEMID + i;
        econItem->originalID = 0;
        econItem->accountID = localInventory->getAccountID();
        econItem->inventory = baseInvID + i + 1;
        econItem->rarity = item.rarity;
        econItem->quality = 4;
        econItem->weaponId = item.weaponID;

        if (item.isSticker() || item.isPatch() || item.isGraffiti() || item.isSealedGraffiti()) {
            econItem->setStickerID(0, StaticData::paintKits()[item.dataIndex].id);
        } else if (item.isMusic()) {
            econItem->setMusicID(StaticData::paintKits()[item.dataIndex].id);
            const auto& dynamicData = dynamicMusicData[inventory[i].getDynamicDataIndex()];
            if (dynamicData.statTrak > -1) {
                econItem->setStatTrak(dynamicData.statTrak);
                econItem->setStatTrakType(1);
                econItem->quality = 9;
            }
        } else if (item.isSkin()) {
            econItem->setPaintKit(static_cast<float>(StaticData::paintKits()[item.dataIndex].id));

            const auto& dynamicData = dynamicSkinData[inventory[i].getDynamicDataIndex()];
            if (dynamicData.isSouvenir) {
                econItem->quality = 12;
            } else {
                if (dynamicData.statTrak > -1) {
                    econItem->setStatTrak(dynamicData.statTrak);
                    econItem->setStatTrakType(0);
                    econItem->quality = 9;
                }
                if (isKnife(econItem->weaponId))
                    econItem->quality = 3;
            }

            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
            memory->setCustomName(econItem, dynamicData.nameTag.c_str());

            for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
                const auto& sticker = dynamicData.stickers[j];
                if (sticker.stickerID == 0)
                    continue;

                econItem->setStickerID(j, sticker.stickerID);
                econItem->setStickerWear(j, sticker.wear);
            }
        } else if (item.isGlove()) {
            econItem->quality = 3;
            econItem->setPaintKit(static_cast<float>(StaticData::paintKits()[item.dataIndex].id));

            const auto& dynamicData = dynamicGloveData[inventory[i].getDynamicDataIndex()];
            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
        } else if (item.isCollectible()) {
            if (StaticData::collectibles()[item.dataIndex].isOriginal)
                econItem->quality = 1;
        } else if (item.isAgent()) {
            const auto& dynamicData = dynamicAgentData[inventory[i].getDynamicDataIndex()];
            for (std::size_t j = 0; j < dynamicData.patches.size(); ++j) {
                const auto& patch = dynamicData.patches[j];
                if (patch.patchID == 0)
                    continue;

                econItem->setStickerID(j, patch.patchID);
            }
        }

        baseTypeCache->addObject(econItem);
        memory->addEconItem(localInventory, econItem, false, false, false);

        if (const auto inventoryComponent = *memory->uiComponentInventory) {
            memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "recent", "0");
            memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "updated", "0");
        }

        if (const auto view = memory->findOrCreateEconItemViewForItemID(econItem->itemID))
            memory->clearInventoryImageRGBA(view);
    }

    for (const auto& item : toEquip)
        memory->inventoryManager->equipItemInSlot(item.team, item.slot, item.index + BASE_ITEMID);

    toEquip.clear();

 //   if (inventoryUpdated)
 //       sendInventoryUpdatedEvent();

    ToolUser::postAddItems();
}

void InventoryChanger::scheduleHudUpdate() noexcept
{
    interfaces->cvar->findVar("cl_fullupdate")->changeCallback();
    hudUpdateRequired = true;
}

void InventoryChanger::overrideHudIcon(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (event.getInt("attacker") != localPlayer->getUserId())
        return;

    if (const auto weapon = std::string_view{ event.getString("weapon") }; weapon != "knife" && weapon != "knife_t")
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 0);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc || Inventory::getItem(soc->itemID) == nullptr)
        return;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(soc->weaponId)) {
        if (const auto defName = def->getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
            event.setString("weapon", defName + 7);
    }
}

void InventoryChanger::updateStatTrak(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    const auto itemID = weapon->itemID();
    const auto item = Inventory::getItem(itemID);
    if (!item || !item->isSkin())
        return;

    const auto itemView = memory->getInventoryItemByItemID(localInventory, itemID);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    auto& dynamicData = dynamicSkinData[item->getDynamicDataIndex()];
    if (dynamicData.statTrak > -1) {
        ++dynamicData.statTrak;
        soc->setStatTrak(dynamicData.statTrak);
        localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
    }
}

void InventoryChanger::onRoundMVP(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("userid") != localUserId)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(Team::None, 54);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc)
        return;

    const auto item = Inventory::getItem(soc->itemID);
    if (!item || !item->isMusic())
        return;

    auto& dynamicData = dynamicMusicData[item->getDynamicDataIndex()];
    if (dynamicData.statTrak > -1) {
        ++dynamicData.statTrak;
        event.setInt("musickitmvps", dynamicData.statTrak);
        soc->setStatTrak(dynamicData.statTrak);
        localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)soc, 4);
    }
}

static bool windowOpen = false;

void InventoryChanger::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Inventory Changer")) {
        windowOpen = true;
        ImGui::SetWindowFocus("Inventory Changer");
        ImGui::SetWindowPos("Inventory Changer", { 100.0f, 100.0f });
    }
}

void InventoryChanger::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Inventory Changer")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

static ImTextureID getItemIconTexture(const std::string& iconpath) noexcept;

namespace ImGui
{
    static bool SkinSelectable(const StaticData::GameItem& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool selected, int* toAddCount = nullptr) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = StaticData::getWeaponName(item.weaponID).c_str();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = item.hasPaintKit() ? StaticData::paintKits()[item.dataIndex].name.c_str() : "";
        const auto paintKitNameSize = CalcTextSize(paintKitName, nullptr);

        PushID(itemName);
        PushID(paintKitName);
        const auto id = window->GetID(0);
        PopID();
        PopID();

        const auto height = ImMax(paintKitNameSize.y, ImMax(itemNameSize.y, iconSizeSmall.y));
        const auto rarityBulletRadius = IM_FLOOR(height * 0.2f);
        const auto size = ImVec2{ iconSizeSmall.x + rarityBulletRadius * 2.0f + itemNameSize.x + paintKitNameSize.x, height };
        
        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        const auto smallIconMin = pos;
        const auto smallIconMax = smallIconMin + iconSizeSmall;

        const auto rarityBulletPos = ImVec2{ pos.x + iconSizeSmall.x + 5.0f + rarityBulletRadius, pos.y + IM_FLOOR(size.y * 0.5f) };

        const auto itemNameMin = ImVec2{ rarityBulletPos.x + rarityBulletRadius + 5.0f, pos.y };
        const auto itemNameMax = itemNameMin + ImVec2{ itemNameSize.x, size.y };
        
        const auto separatorHeightInv = IM_FLOOR(height * 0.2f);
        const auto separatorMin = ImVec2{ itemNameMax.x + 5.0f, pos.y + separatorHeightInv };
        const auto separatorMax = separatorMin + ImVec2{ 1.0f, height - 2.0f * separatorHeightInv };

        const auto paintKitNameMin = ImVec2{ separatorMax.x + 5.0f, pos.y };
        const auto paintKitNameMax = paintKitNameMin + ImVec2{ paintKitNameSize.x, size.y };

        // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
        ImRect bb(pos, pos + ImVec2{ ImMax(size.x, window->WorkRect.Max.x - pos.x), size.y });
        const float spacingX = style.ItemSpacing.x;
        const float spacingY = style.ItemSpacing.y;
        const float spacingL = IM_FLOOR(spacingX * 0.50f);
        const float spacingU = IM_FLOOR(spacingY * 0.50f);
        bb.Min.x -= spacingL;
        bb.Min.y -= spacingU;
        bb.Max.x += (spacingX - spacingL);
        bb.Max.y += (spacingY - spacingU);

        if (!ItemAdd(bb, id))
            return false;

        const ImRect selectableBB{ bb.Min, ImVec2{ bb.Max.x - (selected ? 90.0f : 0.0f), bb.Max.y} };
        // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
        ImGuiButtonFlags buttonFlags = 0;
        bool hovered, held;
        bool pressed = ButtonBehavior(selectableBB, id, &hovered, &held, buttonFlags);

        // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
        if (pressed) {
            if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent) {
                SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImRect(bb.Min - window->Pos, bb.Max - window->Pos));
                g.NavDisableHighlight = true;
            }
            MarkItemEdited(id);
        }

        if (hovered || selected) {
            const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        if (const auto icon = getItemIconTexture(item.iconPath)) {
            window->DrawList->AddImage(icon, smallIconMin, smallIconMax);
            if (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
                BeginTooltip();
                Image(icon, iconSizeLarge);
                EndTooltip();
            }
        }

        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius + 1.0f, IM_COL32(0, 0, 0, (std::min)(120u, (rarityColor & IM_COL32_A_MASK))), 12);
        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius, rarityColor, 12);

        RenderTextClipped(itemNameMin, itemNameMax, itemName, nullptr, &itemNameSize, { 0.0f, 0.5f }, &bb);
        if (paintKitName[0] != '\0')
            window->DrawList->AddRectFilled(separatorMin, separatorMax, GetColorU32(ImGuiCol_Text));
        RenderTextClipped(paintKitNameMin, paintKitNameMax, paintKitName, nullptr, &paintKitNameSize, { 0.0f, 0.5f }, &bb);

        if (selected && toAddCount) {
            const auto cursorPosNext = window->DC.CursorPos.y;
            SameLine(window->WorkRect.Max.x - pos.x - 90.0f);
            const auto cursorPosBackup = window->DC.CursorPos.y;

            window->DC.CursorPos.y += (size.y - GetFrameHeight()) * 0.5f;
            SetNextItemWidth(80.0f);
            InputInt("", toAddCount);
            *toAddCount = (std::max)(*toAddCount, 1);

            window->DC.CursorPosPrevLine.y = cursorPosBackup;
            window->DC.CursorPos.y = cursorPosNext;
        }

        if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
            CloseCurrentPopup();

        return pressed;
    }

    static void SkinItem(const StaticData::GameItem& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool& shouldDelete) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const auto itemName = StaticData::getWeaponName(item.weaponID).c_str();
        const auto itemNameSize = CalcTextSize(itemName, nullptr);

        const auto paintKitName = item.hasPaintKit() ? StaticData::paintKits()[item.dataIndex].name.c_str() : "";
        const auto paintKitNameSize = CalcTextSize(paintKitName, nullptr);

        PushID(itemName);
        PushID(paintKitName);
        const auto id = window->GetID(0);
        PopID();
        PopID();

        const auto height = ImMax(paintKitNameSize.y, ImMax(itemNameSize.y, iconSizeSmall.y));
        const auto rarityBulletRadius = IM_FLOOR(height * 0.2f);
        const auto size = ImVec2{ iconSizeSmall.x + rarityBulletRadius * 2.0f + itemNameSize.x + paintKitNameSize.x, height };

        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        const auto smallIconMin = pos;
        const auto smallIconMax = smallIconMin + iconSizeSmall;

        const auto rarityBulletPos = ImVec2{ pos.x + iconSizeSmall.x + 5.0f + rarityBulletRadius, pos.y + IM_FLOOR(size.y * 0.5f) };

        const auto itemNameMin = ImVec2{ rarityBulletPos.x + rarityBulletRadius + 5.0f, pos.y };
        const auto itemNameMax = itemNameMin + ImVec2{ itemNameSize.x, size.y };

        const auto separatorHeightInv = IM_FLOOR(height * 0.2f);
        const auto separatorMin = ImVec2{ itemNameMax.x + 5.0f, pos.y + separatorHeightInv };
        const auto separatorMax = separatorMin + ImVec2{ 1.0f, height - 2.0f * separatorHeightInv };

        const auto paintKitNameMin = ImVec2{ separatorMax.x + 5.0f, pos.y };
        const auto paintKitNameMax = paintKitNameMin + ImVec2{ paintKitNameSize.x, size.y };

        // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
        ImRect bb(pos, pos + ImVec2{ ImMax(size.x, window->WorkRect.Max.x - pos.x), size.y });
        const float spacingX = style.ItemSpacing.x;
        const float spacingY = style.ItemSpacing.y;
        const float spacingL = IM_FLOOR(spacingX * 0.50f);
        const float spacingU = IM_FLOOR(spacingY * 0.50f);
        bb.Min.x -= spacingL;
        bb.Min.y -= spacingU;
        bb.Max.x += (spacingX - spacingL);
        bb.Max.y += (spacingY - spacingU);

        if (!ItemAdd(bb, id))
            return;

        if (const bool hovered = (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, bb.Max))) {
            const ImU32 col = GetColorU32(ImGuiCol_HeaderHovered);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        if (const auto icon = getItemIconTexture(item.iconPath)) {
            window->DrawList->AddImage(icon, smallIconMin, smallIconMax);
            if (g.HoveredWindow == window && IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
                BeginTooltip();
                Image(icon, iconSizeLarge);
                EndTooltip();
            }
        }

        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius + 1.0f, IM_COL32(0, 0, 0, (std::min)(120u, (rarityColor & IM_COL32_A_MASK))), 12);
        window->DrawList->AddCircleFilled(rarityBulletPos, rarityBulletRadius, rarityColor, 12);

        RenderTextClipped(itemNameMin, itemNameMax, itemName, nullptr, &itemNameSize, { 0.0f, 0.5f }, &bb);
        if (paintKitName[0] != '\0')
            window->DrawList->AddRectFilled(separatorMin, separatorMax, GetColorU32(ImGuiCol_Text));
        RenderTextClipped(paintKitNameMin, paintKitNameMax, paintKitName, nullptr, &paintKitNameSize, { 0.0f, 0.5f }, &bb);

        const auto removeButtonSize = CalcTextSize("Delete", nullptr) + style.FramePadding * 2.0f;
        const auto cursorPosNext = window->DC.CursorPos.y;
        SameLine(window->WorkRect.Max.x - pos.x - removeButtonSize.x - 7.0f);
        const auto cursorPosBackup = window->DC.CursorPos.y;

        window->DC.CursorPos.y += (size.y - GetFrameHeight()) * 0.5f;
        if (Button("Delete"))
            shouldDelete = true;

        window->DC.CursorPosPrevLine.y = cursorPosBackup;
        window->DC.CursorPos.y = cursorPosNext;
    }
}

void InventoryChanger::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!windowOpen)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 400.0f });
        if (!ImGui::Begin("Inventory Changer", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            ImGui::End();
            return;
        }
    }

    static std::string filter;

    static bool isInAddMode = false;

    if (!isInAddMode && ImGui::Button("Add items.."))
        isInAddMode = true;

    if (!isInAddMode) {
        ImGui::SameLine();
        if (ImGui::Button("Force Update"))
            InventoryChanger::scheduleHudUpdate();
    }

    constexpr auto rarityColor = [](int rarity) noexcept {
        constexpr auto rarityColors = std::to_array<ImU32>({
            IM_COL32(0,   0,   0,   0),
            IM_COL32(176, 195, 217, 255),
            IM_COL32(94, 152, 217, 255),
            IM_COL32(75, 105, 255, 255),
            IM_COL32(136,  71, 255, 255),
            IM_COL32(211,  44, 230, 255),
            IM_COL32(235,  75,  75, 255),
            IM_COL32(228, 174,  57, 255)
            });
        return rarityColors[static_cast<std::size_t>(rarity) < rarityColors.size() ? rarity : 0];
    };

    if (isInAddMode) {
        static std::unordered_map<std::size_t, int> selectedToAdd;
        if (ImGui::Button("Back")) {
            isInAddMode = false;
            selectedToAdd.clear();
        }
        ImGui::SameLine();
        const auto canAdd = !selectedToAdd.empty();
        if (!canAdd) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        if (ImGui::Button(("Add selected (" + std::to_string(selectedToAdd.size()) + ")").c_str())) {
            isInAddMode = false;
            addToInventory(selectedToAdd);
            selectedToAdd.clear();
        }
        if (!canAdd) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##search", "Search weapon skins, stickers, knives, gloves, music kits..", &filter);

        constexpr auto passesFilter = [](const std::wstring& str, std::wstring filter) {
            constexpr auto delimiter = L" ";
            wchar_t* _;
            wchar_t* token = std::wcstok(filter.data(), delimiter, &_);
            while (token) {
                if (!std::wcsstr(str.c_str(), token))
                    return false;
                token = std::wcstok(nullptr, delimiter, &_);
            }
            return true;
        };

        if (ImGui::BeginChild("##scrollarea", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            const auto& gameItems = StaticData::gameItems();
            const std::wstring filterWide = Helpers::toUpper(Helpers::toWideString(filter));
            for (std::size_t i = 0; i < gameItems.size(); ++i) {
                if (!filter.empty() && !passesFilter(StaticData::getWeaponNameUpper(gameItems[i].weaponID), filterWide) && (!gameItems[i].hasPaintKit() || !passesFilter(StaticData::paintKits()[gameItems[i].dataIndex].nameUpperCase, filterWide)))
                    continue;
                ImGui::PushID(i);

                const auto selected = selectedToAdd.contains(i);
                const auto toAddCount = selected ? &selectedToAdd[i] : nullptr;

                if (ImGui::SkinSelectable(gameItems[i], { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(gameItems[i].rarity), selected, toAddCount)) {
                    if (selected)
                        selectedToAdd.erase(i);
                    else
                        selectedToAdd.emplace(i, 1);
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    } else {
        if (ImGui::BeginChild("##scrollarea", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f })) {
            for (std::size_t i = inventory.size(); i-- > 0;) {
                if (inventory[i].isDeleted() || inventory[i].shouldDelete())
                    continue;

                ImGui::PushID(i);
                bool shouldDelete = false;
                ImGui::SkinItem(inventory[i].get(), { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(inventory[i].get().rarity), shouldDelete);
                if (shouldDelete)
                    inventory[i].markToDelete();
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }

    if (!contentOnly)
        ImGui::End();
}

json InventoryChanger::toJson() noexcept
{
    json j;

    j["Version"] = 1;

    auto& items = j["Items"];
    for (const auto& item : inventory) {
        if (item.isDeleted())
            continue;

        json itemConfig;

        const auto& gameItem = item.get();
        switch (gameItem.type) {
        case StaticData::Type::Sticker: {
            itemConfig["Type"] = "Sticker";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Sticker ID"] = staticData.id;
            break;
        }
        case StaticData::Type::Glove: {
            itemConfig["Type"] = "Glove";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;

            const auto& dynamicData = dynamicGloveData[item.getDynamicDataIndex()];

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
            break;
        }
        case StaticData::Type::Skin: {
            itemConfig["Type"] = "Skin";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Weapon ID"] = gameItem.weaponID;

            const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

            if (dynamicData.isSouvenir)
                itemConfig["Is Souvenir"] = dynamicData.isSouvenir;
            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
            if (dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;

            if (!dynamicData.nameTag.empty())
                itemConfig["Name Tag"] = dynamicData.nameTag;

            auto& stickers = itemConfig["Stickers"];
            for (std::size_t i = 0; i < dynamicData.stickers.size(); ++i) {
                const auto& sticker = dynamicData.stickers[i];
                if (sticker.stickerID == 0)
                    continue;

                json stickerConfig;
                stickerConfig["Sticker ID"] = sticker.stickerID;
                stickerConfig["Wear"] = sticker.wear;
                stickerConfig["Slot"] = i;
                stickers.push_back(std::move(stickerConfig));
            }
            break;
        }
        case StaticData::Type::Music: {
            itemConfig["Type"] = "Music";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Music ID"] = staticData.id;
            const auto& dynamicData = dynamicMusicData[item.getDynamicDataIndex()];
            if (dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;
            break;
        }
        case StaticData::Type::Collectible: {
            itemConfig["Type"] = "Collectible";
            const auto& staticData = StaticData::collectibles()[gameItem.dataIndex];
            itemConfig["Weapon ID"] = gameItem.weaponID;
            itemConfig["Is Original"] = staticData.isOriginal;
            break;
        }
        case StaticData::Type::NameTag: {
            itemConfig["Type"] = "Name Tag";
            break;
        }
        case StaticData::Type::Patch: {
            itemConfig["Type"] = "Patch";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Patch ID"] = staticData.id;
            break;
        }
        case StaticData::Type::Graffiti: {
            itemConfig["Type"] = "Graffiti";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Graffiti ID"] = staticData.id;
            break;
        }
        case StaticData::Type::SealedGraffiti: {
            itemConfig["Type"] = "Sealed Graffiti";
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Graffiti ID"] = staticData.id;
            break;
        }
        case StaticData::Type::Agent: {
            itemConfig["Type"] = "Agent";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            
            const auto& dynamicData = dynamicAgentData[item.getDynamicDataIndex()];
            auto& stickers = itemConfig["Patches"];
            for (std::size_t i = 0; i < dynamicData.patches.size(); ++i) {
                const auto& patch = dynamicData.patches[i];
                if (patch.patchID == 0)
                    continue;

                json patchConfig;
                patchConfig["Patch ID"] = patch.patchID;
                patchConfig["Slot"] = i;
                stickers.push_back(std::move(patchConfig));
            }
            break;
        }
        case StaticData::Type::Case: {
            itemConfig["Type"] = "Case";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::CaseKey: {
            itemConfig["Type"] = "Case Key";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        case StaticData::Type::OperationPass: {
            itemConfig["Type"] = "Operation Pass";
            itemConfig["Weapon ID"] = gameItem.weaponID;
            break;
        }
        }

        items.push_back(std::move(itemConfig));
    }

    if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
        auto& equipment = j["Equipment"];
        for (std::size_t i = 0; i < 57; ++i) {
            json slot;

            if (const auto itemCT = localInventory->getItemInLoadout(Team::CT, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemCT); soc && Inventory::getItem(soc->itemID))
                    slot["CT"] = static_cast<std::size_t>(soc->itemID - BASE_ITEMID - std::count_if(inventory.begin(), inventory.begin() + static_cast<std::size_t>(soc->itemID - BASE_ITEMID), [](const auto& item) { return item.isDeleted(); }));
            }

            if (const auto itemTT = localInventory->getItemInLoadout(Team::TT, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemTT); soc && Inventory::getItem(soc->itemID))
                    slot["TT"] = static_cast<std::size_t>(soc->itemID - BASE_ITEMID - std::count_if(inventory.begin(), inventory.begin() + static_cast<std::size_t>(soc->itemID - BASE_ITEMID), [](const auto& item) { return item.isDeleted(); }));
            }

            if (const auto itemNOTEAM = localInventory->getItemInLoadout(Team::None, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemNOTEAM); soc && Inventory::getItem(soc->itemID))
                    slot["NOTEAM"] = static_cast<std::size_t>(soc->itemID - BASE_ITEMID - std::count_if(inventory.begin(), inventory.begin() + static_cast<std::size_t>(soc->itemID - BASE_ITEMID), [](const auto& item) { return item.isDeleted(); }));
            }

            if (!slot.empty()) {
                slot["Slot"] = i;
                equipment.push_back(std::move(slot));
            }
        }
    }

    return j;
}

void InventoryChanger::fromJson(const json& j) noexcept
{
    if (!j.contains("Items"))
        return;

    const auto& items = j["Items"];
    if (!items.is_array())
        return;

    for (std::size_t i = 0; i < items.size(); ++i) {
        const auto& jsonItem = items[i];
        if (jsonItem.empty() || !jsonItem.is_object())
            continue;

        if (!jsonItem.contains("Type") || !jsonItem["Type"].is_string())
            continue;

        const std::string type = jsonItem["Type"];
        if (type == "Sticker") {
            if (!jsonItem.contains("Sticker ID") || !jsonItem["Sticker ID"].is_number_integer())
                continue;

            const int stickerID = jsonItem["Sticker ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [stickerID](const auto& gameItem) { return gameItem.isSticker() && StaticData::paintKits()[gameItem.dataIndex].id == stickerID; });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Skin") {
            if (!jsonItem.contains("Paint Kit") || !jsonItem["Paint Kit"].is_number_integer())
                continue;

            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const int paintKit = jsonItem["Paint Kit"];
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [paintKit, weaponID](const auto& gameItem) { return gameItem.isSkin() && StaticData::paintKits()[gameItem.dataIndex].id == paintKit && gameItem.weaponID == weaponID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            DynamicSkinData dynamicData;

            if (jsonItem.contains("Is Souvenir")) {
                if (const auto& isSouvenir = jsonItem["Is Souvenir"]; isSouvenir.is_boolean())
                    dynamicData.isSouvenir = isSouvenir;
            }

            if (jsonItem.contains("Wear")) {
                if (const auto& wear = jsonItem["Wear"]; wear.is_number_float())
                    dynamicData.wear = wear;
            }

            if (jsonItem.contains("Seed")) {
                if (const auto& seed = jsonItem["Seed"]; seed.is_number_integer())
                    dynamicData.seed = seed;
            }

            if (jsonItem.contains("StatTrak")) {
                if (const auto& statTrak = jsonItem["StatTrak"]; statTrak.is_number_integer())
                    dynamicData.statTrak = statTrak;
            }

            if (jsonItem.contains("Name Tag")) {
                if (const auto& nameTag = jsonItem["Name Tag"]; nameTag.is_string())
                    dynamicData.nameTag = nameTag;
            }

            if (jsonItem.contains("Stickers")) {
                if (const auto& stickers = jsonItem["Stickers"]; stickers.is_array()) {
                    for (std::size_t k = 0; k < stickers.size(); ++k) {
                        const auto& sticker = stickers[k];
                        if (!sticker.is_object())
                            continue;

                        if (!sticker.contains("Sticker ID") || !sticker["Sticker ID"].is_number_integer())
                            continue;

                        if (!sticker.contains("Slot") || !sticker["Slot"].is_number_integer())
                            continue;

                        const int stickerID = sticker["Sticker ID"];
                        if (stickerID == 0)
                            continue;
                        const std::size_t slot = sticker["Slot"];
                        if (slot >= std::tuple_size_v<decltype(DynamicSkinData::stickers)>)
                            continue;
                        dynamicData.stickers[slot].stickerID = stickerID;
                        if (sticker.contains("Wear") && sticker["Wear"].is_number_float())
                            dynamicData.stickers[slot].wear = sticker["Wear"];
                    }
                }
            }

            dynamicSkinData.push_back(std::move(dynamicData));
            Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), dynamicSkinData.size() - 1, false);
        } else if (type == "Glove") {
            if (!jsonItem.contains("Paint Kit") || !jsonItem["Paint Kit"].is_number_integer())
                continue;

            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const int paintKit = jsonItem["Paint Kit"];
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [paintKit, weaponID](const auto& gameItem) { return gameItem.isGlove() && StaticData::paintKits()[gameItem.dataIndex].id == paintKit && gameItem.weaponID == weaponID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            DynamicGloveData dynamicData;

            if (jsonItem.contains("Wear")) {
                if (const auto& wear = jsonItem["Wear"]; wear.is_number_float())
                    dynamicData.wear = wear;
            }

            if (jsonItem.contains("Seed")) {
                if (const auto& seed = jsonItem["Seed"]; seed.is_number_integer())
                    dynamicData.seed = seed;
            }

            dynamicGloveData.push_back(std::move(dynamicData));
            Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), dynamicGloveData.size() - 1, false);
        } else if (type == "Music") {
            if (!jsonItem.contains("Music ID") || !jsonItem["Music ID"].is_number_integer())
                continue;

            const int musicID = jsonItem["Music ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [musicID](const auto& gameItem) { return gameItem.isMusic() && StaticData::paintKits()[gameItem.dataIndex].id == musicID; });

            if (staticData == StaticData::gameItems().end())
                continue;

            DynamicMusicData dynamicData;

            if (jsonItem.contains("StatTrak")) {
                if (const auto& statTrak = jsonItem["StatTrak"]; statTrak.is_number_integer() && statTrak > -1)
                    dynamicData.statTrak = statTrak;
            }

            dynamicMusicData.push_back(dynamicData);
            Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), dynamicMusicData.size() - 1, false);
        } else if (type == "Collectible") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;
            if (!jsonItem.contains("Is Original") || !jsonItem["Is Original"].is_boolean())
                continue;

            const WeaponId weaponID = jsonItem["Weapon ID"];
            const bool isOriginal = jsonItem["Is Original"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [weaponID, isOriginal](const auto& gameItem) { return gameItem.isCollectible() && gameItem.weaponID == weaponID && StaticData::collectibles()[gameItem.dataIndex].isOriginal == isOriginal; });

            if (staticData == StaticData::gameItems().end())
                continue;

            Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Name Tag") {
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [](const auto& gameItem) { return gameItem.isNameTag(); });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Patch") {
            if (!jsonItem.contains("Patch ID") || !jsonItem["Patch ID"].is_number_integer())
                continue;

            const int patchID = jsonItem["Patch ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [patchID](const auto& gameItem) { return gameItem.isPatch() && StaticData::paintKits()[gameItem.dataIndex].id == patchID; });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Graffiti") {
            if (!jsonItem.contains("Graffiti ID") || !jsonItem["Graffiti ID"].is_number_integer())
                continue;

            const int graffitiID = jsonItem["Graffiti ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [graffitiID](const auto& gameItem) { return gameItem.isGraffiti() && StaticData::paintKits()[gameItem.dataIndex].id == graffitiID; });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Sealed Graffiti") {
            if (!jsonItem.contains("Graffiti ID") || !jsonItem["Graffiti ID"].is_number_integer())
                continue;

            const int graffitiID = jsonItem["Graffiti ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [graffitiID](const auto& gameItem) { return gameItem.isSealedGraffiti() && StaticData::paintKits()[gameItem.dataIndex].id == graffitiID; });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Agent") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;
            
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [weaponID](const auto& gameItem) { return gameItem.isAgent() && gameItem.weaponID == weaponID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            DynamicAgentData dynamicData;

            if (jsonItem.contains("Patches")) {
                if (const auto& patches = jsonItem["Patches"]; patches.is_array()) {
                    for (std::size_t k = 0; k < patches.size(); ++k) {
                        const auto& patch = patches[k];
                        if (!patch.is_object())
                            continue;

                        if (!patch.contains("Patch ID") || !patch["Patch ID"].is_number_integer())
                            continue;

                        if (!patch.contains("Slot") || !patch["Slot"].is_number_integer())
                            continue;

                        const int patchID = patch["Patch ID"];
                        if (patchID == 0)
                            continue;
                        const std::size_t slot = patch["Slot"];
                        if (slot >= std::tuple_size_v<decltype(DynamicAgentData::patches)>)
                            continue;
                        dynamicData.patches[slot].patchID = patchID;
                    }
                }
            }

            dynamicAgentData.push_back(std::move(dynamicData));
            Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), dynamicAgentData.size() - 1, false);
        } else if (type == "Case") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [weaponID](const auto& gameItem) { return gameItem.isCase() && gameItem.weaponID == weaponID; });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Case Key") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [weaponID](const auto& gameItem) { return gameItem.isCaseKey() && gameItem.weaponID == weaponID; });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        } else if (type == "Operation Pass") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [weaponID](const auto& gameItem) { return gameItem.isOperationPass() && gameItem.weaponID == weaponID; });
            if (staticData != StaticData::gameItems().end())
                Inventory::addItem(std::ranges::distance(StaticData::gameItems().begin(), staticData), Inventory::INVALID_DYNAMIC_DATA_IDX, false);
        }
    }

    if (!j.contains("Equipment"))
        return;

    const auto& equipment = j["Equipment"];
    if (!equipment.is_array())
        return;

    for (std::size_t i = 0; i < equipment.size(); ++i) {
        const auto& equipped = equipment[i];
        if (!equipped.contains("Slot"))
            continue;

        const auto& slot = equipped["Slot"];
        if (!slot.is_number_integer())
            continue;

        if (equipped.contains("CT")) {
            if (const auto& ct = equipped["CT"]; ct.is_number_integer())
                toEquip.emplace_back(Team::CT, slot, ct);
        }

        if (equipped.contains("TT")) {
            if (const auto& tt = equipped["TT"]; tt.is_number_integer())
                toEquip.emplace_back(Team::TT, slot, tt);
        }

        if (equipped.contains("NOTEAM")) {
            if (const auto& noteam = equipped["NOTEAM"]; noteam.is_number_integer())
                toEquip.emplace_back(Team::None, slot, noteam);
        }
    }
}

void InventoryChanger::resetConfig() noexcept
{
    Inventory::clear();
    dynamicSkinData.clear();
}

void InventoryChanger::clearInventory() noexcept
{
    resetConfig();
}

static std::size_t lastEquippedCount = 0;
void InventoryChanger::onItemEquip(Team team, int slot, std::uint64_t itemID) noexcept
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto item = Inventory::getItem(itemID);
    if (!item)
        return;

    if (item->isCollectible()) {
        if (const auto view = memory->getInventoryItemByItemID(localInventory, itemID)) {
            if (const auto econItem = memory->getSOCData(view))
                localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
        }
    } else if (item->isSkin()) {
        const auto view = localInventory->getItemInLoadout(team, slot);
        memory->inventoryManager->equipItemInSlot(team, slot, (std::uint64_t(0xF) << 60) | static_cast<short>(item->get().weaponID));
        if (view) {
            if (const auto econItem = memory->getSOCData(view))
                localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
        }
        ++lastEquippedCount;
    }
}

void InventoryChanger::onSoUpdated(SharedObject* object, int event) noexcept
{
    if (lastEquippedCount > 0 && object->getTypeID() == 43 /* = k_EEconTypeDefaultEquippedDefinitionInstanceClient */) {
        *reinterpret_cast<WeaponId*>(std::uintptr_t(object) + WIN32_LINUX(0x10, 0x1C)) = WeaponId::None;
        --lastEquippedCount;
    }
}

struct Icon {
    Texture texture;
    int lastReferencedFrame = 0;
};

static std::unordered_map<std::string, Icon> iconTextures;

static ImTextureID getItemIconTexture(const std::string& iconpath) noexcept
{
    if (iconpath.empty())
        return 0;

    auto& icon = iconTextures[iconpath];
    if (!icon.texture.get()) {
        static int frameCount = 0;
        static float timeSpentThisFrame = 0.0f;
        static int loadedThisFrame = 0;

        if (frameCount != ImGui::GetFrameCount()) {
            frameCount = ImGui::GetFrameCount();
            timeSpentThisFrame = 0.0f;
            // memory->debugMsg("LOADED %d ICONS\n", loadedThisFrame);
            loadedThisFrame = 0;
        }

        if (timeSpentThisFrame > 0.01f)
            return 0;

        ++loadedThisFrame;

        const auto start = std::chrono::high_resolution_clock::now();

        auto handle = interfaces->baseFileSystem->open(("resource/flash/" + iconpath + (iconpath.find("status_icons") != std::string::npos ? "" : "_large") + ".png").c_str(), "r", "GAME");
        if (!handle)
            handle = interfaces->baseFileSystem->open(("resource/flash/" + iconpath + ".png").c_str(), "r", "GAME");

        assert(handle);
        if (handle) {
            if (const auto size = interfaces->baseFileSystem->size(handle); size > 0) {
                const auto buffer = std::make_unique<std::uint8_t[]>(size);
                if (interfaces->baseFileSystem->read(buffer.get(), size, handle) > 0) {
                    int width, height;
                    stbi_set_flip_vertically_on_load_thread(false);

                    if (const auto data = stbi_load_from_memory((const stbi_uc*)buffer.get(), size, &width, &height, nullptr, STBI_rgb_alpha)) {
                        icon.texture.init(width, height, data);
                        stbi_image_free(data);
                    } else {
                        assert(false);
                    }
                }
            }
            interfaces->baseFileSystem->close(handle);
        }

        const auto end = std::chrono::high_resolution_clock::now();
        timeSpentThisFrame += std::chrono::duration<float>(end - start).count();
    }
    icon.lastReferencedFrame = ImGui::GetFrameCount();
    return icon.texture.get();
}

void InventoryChanger::clearItemIconTextures() noexcept
{
    iconTextures.clear();
}

void InventoryChanger::clearUnusedItemIconTextures() noexcept
{
    constexpr auto maxIcons = 30;
    const auto frameCount = ImGui::GetFrameCount();
    while (iconTextures.size() > maxIcons) {
        const auto oldestIcon = std::ranges::min_element(iconTextures, [](const auto& a, const auto& b) { return a.second.lastReferencedFrame < b.second.lastReferencedFrame; });
        if (oldestIcon->second.lastReferencedFrame == frameCount)
            break;

        iconTextures.erase(oldestIcon);
    }
}

static int remapKnifeAnim(WeaponId weaponID, const int sequence) noexcept
{
    enum Sequence
    {
        SEQUENCE_DEFAULT_DRAW = 0,
        SEQUENCE_DEFAULT_IDLE1 = 1,
        SEQUENCE_DEFAULT_IDLE2 = 2,
        SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
        SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
        SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
        SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
        SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
        SEQUENCE_DEFAULT_LOOKAT01 = 12,

        SEQUENCE_BUTTERFLY_DRAW = 0,
        SEQUENCE_BUTTERFLY_DRAW2 = 1,
        SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
        SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

        SEQUENCE_FALCHION_IDLE1 = 1,
        SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
        SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
        SEQUENCE_FALCHION_LOOKAT01 = 12,
        SEQUENCE_FALCHION_LOOKAT02 = 13,

        SEQUENCE_DAGGERS_IDLE1 = 1,
        SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
        SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
        SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
        SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

        SEQUENCE_BOWIE_IDLE1 = 1,
    };

    switch (weaponID) {
    case WeaponId::Butterfly:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
            return randomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
        default:
            return sequence + 1;
        }
    case WeaponId::Falchion:
        switch (sequence) {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_FALCHION_IDLE1;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return randomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence - 1;
        }
    case WeaponId::Daggers:
        switch (sequence) {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_DAGGERS_IDLE1;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
            return randomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return randomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
        case SEQUENCE_DEFAULT_HEAVY_HIT1:
        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
        case SEQUENCE_DEFAULT_LOOKAT01:
            return sequence + 3;
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence + 2;
        }
    case WeaponId::Bowie:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_BOWIE_IDLE1;
        default:
            return sequence - 1;
        }
    case WeaponId::Ursus:
    case WeaponId::SkeletonKnife:
    case WeaponId::NomadKnife:
    case WeaponId::Paracord:
    case WeaponId::SurvivalKnife:
        switch (sequence) {
        case SEQUENCE_DEFAULT_DRAW:
            return randomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomInt(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
        default:
            return sequence + 1;
        }
    case WeaponId::Stiletto:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomInt(12, 13);
        }
    case WeaponId::Talon:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return randomInt(14, 15);
        }
    default:
        return sequence;
    }
}

void InventoryChanger::fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept
{
    if (!localPlayer)
        return;

    if (!isKnife(viewModelWeapon->itemDefinitionIndex2()))
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 0);
    if (!itemView)
        return;

    if (const auto soc = memory->getSOCData(itemView); !soc || Inventory::getItem(soc->itemID) == nullptr)
        return;

    sequence = remapKnifeAnim(viewModelWeapon->itemDefinitionIndex2(), sequence);
}

StaticData::GameItem::GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath) noexcept : type{ type }, rarity{ static_cast<std::uint8_t>(rarity) }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ std::move(iconPath) } {}

StaticData::PaintKit::PaintKit(int id, std::wstring&& name, float wearRemapMin, float wearRemapMax) noexcept : id{ id }, wearRemapMin{ wearRemapMin }, wearRemapMax{ wearRemapMax }, nameUpperCase{std::move(name)}
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}
