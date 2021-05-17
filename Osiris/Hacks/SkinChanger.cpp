#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <random>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>

#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imguiCustom.h"
#include "../imgui/imgui_stdlib.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "SkinChanger.h"
#include "../Config.h"
#include "../Texture.h"
#include "../fnv.h"

#include "../SDK/ClassId.h"
#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/Engine.h"
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
#include "../SDK/Platform.h"
#include "../SDK/WeaponId.h"

#include "../Helpers.h"

static constexpr auto is_knife(WeaponId id)
{
    return (id >= WeaponId::Bayonet && id <= WeaponId::SkeletonKnife) || id == WeaponId::KnifeT || id == WeaponId::Knife;
}

static std::vector<SkinChanger::GameItem> gameItems;
static std::vector<SkinChanger::StickerData> stickerData;
static std::vector<SkinChanger::GloveData> gloveData;
static std::vector<SkinChanger::SkinData> skinData;
static std::vector<SkinChanger::MusicData> musicData;
static std::vector<SkinChanger::CollectibleData> collectibleData;

struct StickerConfig {
    int stickerID = 0;
    float wear = 0.0f;
};

struct DynamicSkinData {
    std::array<StickerConfig, 5> stickers;
    std::string nameTag;
};

static std::vector<DynamicSkinData> dynamicSkinData;

constexpr auto BASE_ITEMID = 1152921504606746975;

struct InventoryItem {
private:
    std::size_t itemIndex;
    std::size_t dynamicDataIndex = static_cast<std::size_t>(dynamicDataIndex);
public:
    InventoryItem(std::size_t itemIndex)  noexcept : itemIndex{ itemIndex }
    {
        if (isSkin()) {
            dynamicSkinData.push_back({});
            dynamicDataIndex = dynamicSkinData.size() - 1;
        }
    }

    void markAsDeleted() noexcept { itemIndex = static_cast<std::size_t>(-1); }
    bool isDeleted() const noexcept { return itemIndex == static_cast<std::size_t>(-1); }
    
    bool isSticker() const noexcept { return !isDeleted() && gameItems[itemIndex].isSticker(); }
    bool isSkin() const noexcept { return !isDeleted() && gameItems[itemIndex].isSkin(); }
    bool isGlove() const noexcept { return !isDeleted() && gameItems[itemIndex].isGlove(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(isSkin()); return dynamicDataIndex; }

    SkinChanger::GameItem& get() const noexcept { assert(!isDeleted()); return gameItems[itemIndex]; }
};

static std::vector<InventoryItem> inventory;

static bool wasItemCreatedByOsiris(std::uint64_t itemID) noexcept
{
    return itemID >= BASE_ITEMID && static_cast<std::size_t>(itemID - BASE_ITEMID) < inventory.size();
}

static void addToInventory(const std::unordered_set<std::size_t>& indicesToAdd) noexcept
{
    for (const auto idx : indicesToAdd) {
        inventory.emplace_back(idx);
    }
}

static void sendInventoryUpdatedEvent() noexcept
{
    if (const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_MyPersona_InventoryUpdated")); idx != -1) {
        if (const auto eventPtr = memory->registeredPanoramaEvents->memory[idx].value.makeEvent(nullptr))
            interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(eventPtr);
    }
}

static void initializeKits() noexcept
{
    if (static bool initialized = false; !initialized)
        initialized = true;
    else
        return;

    const auto itemSchema = memory->itemSystem()->getItemSchema();

    struct KitWeapon {
        KitWeapon(int paintKit, WeaponId weaponId, const char* iconPath) noexcept : paintKit{ paintKit }, weaponId{ weaponId }, iconPath{ iconPath } {}
        int paintKit;
        WeaponId weaponId;
        const char* iconPath;
    };

    std::vector<KitWeapon> kitsWeapons;
    kitsWeapons.reserve(itemSchema->alternateIcons.numElements);

    for (const auto& node : itemSchema->alternateIcons) {
        // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_schema.cpp#L325-L329
        if (const auto encoded = node.key; (encoded & 3) == 0)
            kitsWeapons.emplace_back(int((encoded & 0xFFFF) >> 2), WeaponId(encoded >> 16), node.value.simpleName.data());
    }
    std::ranges::sort(kitsWeapons, {}, &KitWeapon::paintKit);
    
    std::unordered_map<WeaponId, std::wstring> weaponNames;
    for (const auto& kitWeapon : kitsWeapons) {
        if (weaponNames.contains(kitWeapon.weaponId))
            continue;

        if (const auto itemDef = itemSchema->getItemDefinitionInterface(kitWeapon.weaponId))
            weaponNames.emplace(kitWeapon.weaponId, interfaces->localize->findSafe(itemDef->getItemBaseName()));
    }
 
    gameItems.reserve(itemSchema->paintKits.lastAlloc);
    for (const auto& node : itemSchema->paintKits) {
        const auto paintKit = node.value;

        if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
            continue;

        if (paintKit->id >= 10000) {
            std::wstring name;
            std::string iconPath;

            if (const auto it = std::ranges::lower_bound(std::as_const(kitsWeapons), paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id) {
                name = weaponNames[it->weaponId];
                name += L" | ";
                iconPath = it->iconPath;
                name += interfaces->localize->findSafe(paintKit->itemName.data() + 1);

                gloveData.emplace_back(paintKit->id, it->weaponId);
                gameItems.emplace_back(SkinChanger::GameItem::Type::Glove, paintKit->rarity, gloveData.size() - 1, std::move(name), std::move(iconPath));
            }
        } else {
            for (auto it = std::ranges::lower_bound(std::as_const(kitsWeapons), paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {
                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                std::wstring name = weaponNames[it->weaponId];
                name += L" | ";
                name += interfaces->localize->findSafe(paintKit->itemName.data() + 1);
                skinData.emplace_back(paintKit->id, it->weaponId);
                gameItems.emplace_back(SkinChanger::GameItem::Type::Skin, std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6), skinData.size() - 1, std::move(name), it->iconPath);
            }
        }
    }

    const auto& stickerMap = itemSchema->stickerKits;
    gameItems.reserve(gameItems.size() + stickerMap.numElements);
    for (const auto& node : stickerMap) {
        const auto stickerKit = node.value;
        if (stickerKit->id == 0)
            continue;

        if (std::string_view name{ stickerKit->name.data() }; name.starts_with("spray") || name.starts_with("patch") || name.ends_with("graffiti"))
            continue;

        std::wstring name = interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() + 1 : "StickerKit_dhw2014_teamdignitas_gold");
        stickerData.emplace_back(stickerKit->id);
        gameItems.emplace_back(SkinChanger::GameItem::Type::Sticker, stickerKit->rarity, stickerData.size() - 1, std::move(name), stickerKit->inventoryImage.data());
    }

    const auto& musicMap = itemSchema->musicKits;
    for (const auto& node : musicMap) {
        const auto musicKit = node.value;
        if (musicKit->id == 1 || musicKit->id == 2)
            continue;

        musicData.emplace_back(musicKit->id);
        gameItems.emplace_back(SkinChanger::GameItem::Type::Music, 3, musicData.size() - 1, interfaces->localize->findSafe(musicKit->nameLocalized), musicKit->inventoryImage);
    }

    for (const auto& node : itemSchema->itemsSorted) {
        const auto item = node.value;
        if (std::strcmp(item->getItemTypeName(), "#CSGO_Type_Knife") == 0 && item->getRarity() == 6) {
            if (const auto image = item->getInventoryImage()) {
                skinData.emplace_back(0, item->getWeaponId());
                gameItems.emplace_back(SkinChanger::GameItem::Type::Skin, 6, skinData.size() - 1, interfaces->localize->findSafe(item->getItemBaseName()), image);
            }
        }

        if (std::strcmp(item->getItemTypeName(), "#CSGO_Type_Collectible") == 0) {
            if (const auto image = item->getInventoryImage()) {
                collectibleData.emplace_back(item->getWeaponId());
                gameItems.emplace_back(SkinChanger::GameItem::Type::Collectible, item->getRarity(), collectibleData.size() - 1, interfaces->localize->findSafe(item->getItemBaseName()), image);
            }
        }
    }

    std::ranges::sort(gameItems, {}, &SkinChanger::GameItem::nameUpperCase);
    gameItems.shrink_to_fit();
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

static void applyGloves(Entity* local) noexcept
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(local->getTeamNumber(), 41);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    const auto& item = inventory[static_cast<std::size_t>(soc->itemID - BASE_ITEMID)];
    if (!item.isGlove())
        return;

    const auto& itemData = gloveData[item.get().dataIndex];

    const auto wearables = local->wearables();
    static int gloveHandle = 0;

    auto glove = interfaces->entityList->getEntityFromHandle(wearables[0]);
    if (!glove)
        glove = interfaces->entityList->getEntityFromHandle(gloveHandle);
    if (!glove) {
        const auto entry = interfaces->entityList->getHighestEntityIndex() + 1;
        const auto serial = rand() % 0x1000;
        glove = make_glove(entry, serial);
    }

    if (!glove)
        return;

    wearables[0] = gloveHandle = glove->handle();
    glove->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
    glove->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
    glove->initialized() = true;
    memory->equipWearable(glove, local);
    local->body() = 1;

    constexpr auto m_Item = fnv::hash("CBaseAttributableItem->m_Item");
    const auto attributeList = std::uintptr_t(glove) + netvars->operator[](m_Item) + /* m_AttributeList = */ WIN32_LINUX(0x244, 0x2F8);
    memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.paintKit));
    memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", 0.01f);
    memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(1));

    if (auto& definitionIndex = glove->itemDefinitionIndex2(); definitionIndex != itemData.weaponId) {
        definitionIndex = itemData.weaponId;

        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(itemData.weaponId)) {
            glove->setModelIndex(interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel()));
            glove->preDataUpdate(0);
        }
    }
}

static void applyKnife(Entity* local) noexcept
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(local->getTeamNumber(), 0);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    const auto& item = inventory[static_cast<std::size_t>(soc->itemID - BASE_ITEMID)];
    if (!item.isSkin())
        return;

    const auto& itemData = skinData[item.get().dataIndex];

    auto& weapons = local->weapons();

    for (auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;

        auto& definitionIndex = weapon->itemDefinitionIndex2();
        if (!is_knife(definitionIndex))
            continue;

        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);

        constexpr auto m_Item = fnv::hash("CBaseAttributableItem->m_Item");
        const auto attributeList = std::uintptr_t(weapon) + netvars->operator[](m_Item) + /* m_AttributeList = */ WIN32_LINUX(0x244, 0x2F8);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.paintKit));
        memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", 0.01f);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(1));

        const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

        // FIXME: std::strncpy IS UNSAFE!!!
        std::strncpy(weapon->customName(), dynamicData.nameTag.c_str(), 32);

        if (definitionIndex != itemData.weaponId) {
            definitionIndex = itemData.weaponId;

            if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(itemData.weaponId)) {
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

static void applyWeapons(Entity* local) noexcept
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto localTeam = local->getTeamNumber();

    auto& weapons = local->weapons();
    for (auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;

        const auto& definitionIndex = weapon->itemDefinitionIndex2();
        if (is_knife(definitionIndex))
            continue;

        const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(definitionIndex);
        if (!def)
            continue;

        const auto loadoutSlot = def->getLoadoutSlot(localTeam);
        const auto itemView = localInventory->getItemInLoadout(localTeam, loadoutSlot);
        if (!itemView)
            continue;

        const auto soc = memory->getSOCData(itemView);
        if (!soc || !wasItemCreatedByOsiris(soc->itemID))
            continue;

        const auto& item = inventory[static_cast<std::size_t>(soc->itemID - BASE_ITEMID)];
        if (!item.isSkin())
            return;

        const auto& itemData = skinData[item.get().dataIndex];

        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);

        constexpr auto m_Item = fnv::hash("CBaseAttributableItem->m_Item");
        const auto attributeList = std::uintptr_t(weapon) + netvars->operator[](m_Item) + /* m_AttributeList = */ WIN32_LINUX(0x244, 0x2F8);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.paintKit));
        memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", 0.01f);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(1));

        const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

        // FIXME: std::strncpy IS UNSAFE!!!
        std::strncpy(weapon->customName(), dynamicData.nameTag.c_str(), 32);

        for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
            const auto& sticker = dynamicData.stickers[j];
            if (sticker.stickerID == 0)
                continue;

            memory->setOrAddAttributeValueByName(attributeList, ("sticker slot " + std::to_string(j) + " id").c_str(), sticker.stickerID);
            memory->setOrAddAttributeValueByName(attributeList, ("sticker slot " + std::to_string(j) + " wear").c_str(), sticker.wear);
        }
    }
}

static void onPostDataUpdateStart(int localHandle) noexcept
{
    const auto local = interfaces->entityList->getEntityFromHandle(localHandle);
    if (!local)
        return;

    applyGloves(local);
    applyKnife(local);
    applyWeapons(local);
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

static int slotToApplySticker;
void SkinChanger::setStickerApplySlot(int slot) noexcept
{
    slotToApplySticker = slot;
}

static std::uint64_t toolToUse = 0;
static float useToolTime = 0.0f;
void SkinChanger::setToolToUse(std::uint64_t itemID) noexcept
{
    toolToUse = itemID;

    std::mt19937 gen{ std::random_device{}() };
    std::uniform_real_distribution dis{ 0.5f, 0.9f };
    useToolTime = memory->globalVars->realtime + dis(gen);
}

static std::uint64_t itemToApplyTool = 0;
void SkinChanger::setItemToApplyTool(std::uint64_t itemID) noexcept
{
    itemToApplyTool = itemID;
}

static void* initItemCustomizationNotification(const char* typeStr, const char* itemID) noexcept
{
    void* result;

#ifdef _WIN32
    const auto function = memory->initItemCustomizationNotification;
    __asm {
        push itemID
        push typeStr
        xor edx, edx
        xor ecx, ecx
        call function
        add esp, 8
        mov result, eax
    }
#else

#endif
    return result;
}

static void removeItemFromInventory(CSPlayerInventory* inventory, SharedObjectTypeCache<EconItem>* cache, EconItem* econItem) noexcept
{
    inventory->removeItem(econItem->itemID);
    cache->removeObject(econItem);
}

void SkinChanger::run(FrameStage stage) noexcept
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

    static const auto baseInvID = localInventory->getHighestIDs().second;

    if (useToolTime <= memory->globalVars->realtime) {
        if (wasItemCreatedByOsiris(toolToUse) && wasItemCreatedByOsiris(itemToApplyTool)) {
            auto& tool = inventory[static_cast<std::size_t>(toolToUse - BASE_ITEMID)];
            const auto& toolItem = tool.get();
            auto& dest = inventory[static_cast<std::size_t>(itemToApplyTool - BASE_ITEMID)];
            const auto& destItem = dest.get();

            if (toolItem.isSticker()) {
                const auto& sticker = stickerData[toolItem.dataIndex];
                if (const auto view = memory->findOrCreateEconItemViewForItemID(itemToApplyTool)) {
                    memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), ("sticker slot " + std::to_string(slotToApplySticker) + " id").c_str(), sticker.stickerID);
                    memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), ("sticker slot " + std::to_string(slotToApplySticker) + " wear").c_str(), 0.10f);

                    dynamicSkinData[dest.getDynamicDataIndex()].stickers[slotToApplySticker].stickerID = sticker.stickerID;
                    dynamicSkinData[dest.getDynamicDataIndex()].stickers[slotToApplySticker].wear = 0.0f;

                    if (const auto stickerView = memory->findOrCreateEconItemViewForItemID(toolToUse))
                        if (const auto econItem = memory->getSOCData(stickerView))
                            removeItemFromInventory(localInventory, baseTypeCache, econItem);
                    tool.markAsDeleted();

                    const auto event = initItemCustomizationNotification("sticker_apply", std::to_string(itemToApplyTool).c_str());
                    interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(event);
                }
            }
        }
        toolToUse = itemToApplyTool = 0;
    }

    for (std::size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i].isDeleted() || memory->getInventoryItemByItemID(localInventory, BASE_ITEMID + i))
            continue;

        const auto& item = inventory[i].get();
       
        const auto econItem = memory->createEconItemSharedObject();
        econItem->itemID = BASE_ITEMID + i;
        econItem->originalID = 0;
        econItem->accountID = localInventory->getAccountID();
        econItem->inventory = baseInvID + i + 1;
        econItem->rarity = item.rarity;

        switch (item.type) {
        using enum SkinChanger::GameItem::Type;
        case Sticker:
            econItem->weaponId = WeaponId::Sticker;
            break;
        case Skin:
            econItem->weaponId = skinData[item.dataIndex].weaponId;
            break;
        case Glove:
            econItem->weaponId = gloveData[item.dataIndex].weaponId;
            break;
        case Music:
            econItem->weaponId = WeaponId::MusicKit;
            break;
        case Collectible:
            econItem->weaponId = collectibleData[item.dataIndex].weaponId;
            break;
        }

        baseTypeCache->addObject(econItem);
        memory->addEconItem(localInventory, econItem, false, false, false);

        if (const auto view = memory->findOrCreateEconItemViewForItemID(econItem->itemID)) {
            if (item.isSticker()) {
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "sticker slot 0 id", stickerData[item.dataIndex].stickerID);
            } else if (item.isMusic()) {
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "music id", musicData[item.dataIndex].musicID);
            } else if (item.isSkin() || item.isGlove()) {
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "set item texture prefab", static_cast<float>(item.type == SkinChanger::GameItem::Type::Skin ? skinData[item.dataIndex].paintKit : gloveData[item.dataIndex].paintKit));
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "set item texture wear", 0.01f);
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "set item texture seed", static_cast<float>(1));

                if (item.isSkin()) {
                    const auto& dynamicData = dynamicSkinData[inventory[i].getDynamicDataIndex()];

                    memory->setCustomName(econItem, dynamicData.nameTag.c_str());

                    for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
                        const auto& sticker = dynamicData.stickers[j];
                        if (sticker.stickerID == 0)
                            continue;

                        memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), ("sticker slot " + std::to_string(j) + " id").c_str(), sticker.stickerID);
                        memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), ("sticker slot " + std::to_string(j) + " wear").c_str(), sticker.wear);
                    }
                }
            }
            memory->clearInventoryImageRGBA(view);
        }
    }
}

void SkinChanger::scheduleHudUpdate() noexcept
{
    interfaces->cvar->findVar("cl_fullupdate")->changeCallback();
    hudUpdateRequired = true;
}

void SkinChanger::overrideHudIcon(GameEvent& event) noexcept
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
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(soc->weaponId)) {
        if (const auto defName = def->getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
            event.setString("weapon", defName + 7);
    }
}

void SkinChanger::updateStatTrak(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    /*
    if (const auto conf = get_by_definition_index(is_knife(weapon->itemDefinitionIndex2()) ? WeaponId::Knife : weapon->itemDefinitionIndex2()); conf && conf->stat_trak > -1) {
        weapon->fallbackStatTrak() = ++conf->stat_trak;
        weapon->postDataUpdate(0);
    }
    */
}

static bool windowOpen = false;

void SkinChanger::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Skin changer")) {
        windowOpen = true;
        ImGui::SetWindowFocus("Skin changer");
        ImGui::SetWindowPos("Skin changer", { 100.0f, 100.0f });
    }
}

void SkinChanger::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Skin changer")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

namespace ImGui
{
    static bool SkinSelectable(const char* label, const std::string& iconPath, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool selected = false) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        // Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
        ImGuiID id = window->GetID(label);
        ImVec2 label_size = CalcTextSize(label, NULL, true);
        ImVec2 size(label_size.x + iconSizeSmall.x, ImMax(label_size.y, IM_FLOOR(iconSizeSmall.y + 0.99999f)));
        ImVec2 pos = window->DC.CursorPos;
        pos.y += window->DC.CurrLineTextBaseOffset;
        ItemSize(size, 0.0f);

        size.x = ImMax(label_size.x + iconSizeSmall.x, window->WorkRect.Max.x - pos.x);

        // Text stays at the submission position, but bounding box may be extended on both sides
        const ImVec2 text_min = pos + ImVec2{ iconSizeSmall.x, size.y * 0.25f };
        const ImVec2 text_max(pos.x + size.x, pos.y + size.y);

        // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
        ImRect bb(pos.x, pos.y, text_max.x, text_max.y);

        const float spacing_x = style.ItemSpacing.x;
        const float spacing_y = style.ItemSpacing.y;
        const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
        const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
        bb.Min.x -= spacing_L;
        bb.Min.y -= spacing_U;
        bb.Max.x += (spacing_x - spacing_L);
        bb.Max.y += (spacing_y - spacing_U);

        bool item_add = ItemAdd(bb, id);
        if (!item_add)
            return false;

        // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
        ImGuiButtonFlags button_flags = 0;

        const bool was_selected = selected;
        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);

        // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
        if (pressed)
        {
            if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
            {
                SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImRect(bb.Min - window->Pos, bb.Max - window->Pos));
                g.NavDisableHighlight = true;
            }
        }
        if (pressed)
            MarkItemEdited(id);

        // In this branch, Selectable() cannot toggle the selection so this will never trigger.
        if (selected != was_selected) //-V547
            window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

        if (hovered || selected)
        {
            const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        const auto bulletRadius = (bb.Max.y - bb.Min.y) * 0.15f;
        const auto bulletPos = ImVec2{ bb.Min.x + iconSizeSmall.x + bulletRadius + 3.0f, (bb.Min.y + bb.Max.y) * 0.5f };
        window->DrawList->AddCircleFilled(bulletPos, bulletRadius + 1.0f, IM_COL32(0, 0, 0, (std::min)(120u, (rarityColor & IM_COL32_A_MASK))), 12);
        window->DrawList->AddCircleFilled(bulletPos, bulletRadius, rarityColor, 12);

        RenderTextClipped(text_min + ImVec2{ bulletRadius * 2.0f + 4.0f, 0.0f }, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);

        if (const auto icon = SkinChanger::getItemIconTexture(iconPath)) {
            window->DrawList->AddImage(icon, bb.Min, bb.Min + iconSizeSmall, { 0.0f, 0.0f }, { 1.0f, 1.0f }, GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f }));
            if (IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
                BeginTooltip();
                Image(icon, iconSizeLarge);
                EndTooltip();
            }
        }

        if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
            CloseCurrentPopup();

        return pressed;
    }
}

#define SKINCHANGER_REWORK

void SkinChanger::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!windowOpen)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 400.0f });
        if (!ImGui::Begin("Skin changer", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
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
            SkinChanger::scheduleHudUpdate();
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
        static std::unordered_set<std::size_t> selectedToAdd;
        if (ImGui::Button("Back")) {
            isInAddMode = false;
            selectedToAdd.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button(("Add selected (" + std::to_string(selectedToAdd.size()) + ")").c_str())) {
            isInAddMode = false;
            addToInventory(selectedToAdd);
            selectedToAdd.clear();
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

        if (ImGui::BeginChild("##scrollarea")) {
            const auto& gameItems = getGameItems();
            const std::wstring filterWide = Helpers::toUpper(Helpers::toWideString(filter));
            for (std::size_t i = 0; i < gameItems.size(); ++i) {
                if (!filter.empty() && !passesFilter(gameItems[i].nameUpperCase, filterWide))
                    continue;
                ImGui::PushID(i);
                const auto selected = selectedToAdd.contains(i);
                if (ImGui::SkinSelectable(gameItems[i].name.c_str(), gameItems[i].iconPath, { 35.0f, 26.25f }, { 200.0f, 150.0f }, rarityColor(gameItems[i].rarity), selected)) {
                    if (selected)
                        selectedToAdd.erase(i);
                    else
                        selectedToAdd.emplace(i);

                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    } else {
        for (std::size_t i = 0; i < inventory.size(); ++i) {
            if (inventory[i].isDeleted())
                continue;

            ImGui::PushID(i);
            ImGui::Image(getItemIconTexture(inventory[i].get().iconPath), { 100.0f, 75.0f });
            ImGui::PopID();
        }
    }

    if (!contentOnly)
        ImGui::End();
}

json InventoryChanger::toJson() noexcept
{
    json j;
    for (const auto& item : inventory) {
        if (item.isDeleted())
            continue;

        json itemConfig;

        const auto& gameItem = item.get();
        switch (gameItem.type) {
        case SkinChanger::GameItem::Type::Sticker: {
            itemConfig["Type"] = "Sticker";
            const auto& staticData = stickerData[gameItem.dataIndex];
            itemConfig["Sticker ID"] = staticData.stickerID;
            break;
        }
        case SkinChanger::GameItem::Type::Glove: {
            itemConfig["Type"] = "Glove";
            const auto& staticData = gloveData[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.paintKit;
            itemConfig["Weapon ID"] = staticData.weaponId;
            break;
        }
        case SkinChanger::GameItem::Type::Skin: {
            itemConfig["Type"] = "Skin";
            const auto& staticData = skinData[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.paintKit;
            itemConfig["Weapon ID"] = staticData.weaponId;

            const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];
            itemConfig["Name Tag"] = dynamicData.nameTag;

            for (const auto& sticker : dynamicData.stickers) {
                if (sticker.stickerID == 0)
                    continue;

                json stickerConfig;
                stickerConfig["Sticker ID"] = sticker.stickerID;
                stickerConfig["Wear"] = sticker.wear;
                itemConfig["Stickers"].push_back(stickerConfig);
            }
            break;
        }
        }

        j.push_back(itemConfig);
    }
    return j;
}

void InventoryChanger::fromJson(const json& j) noexcept
{
    if (!j.is_array())
        return;

    initializeKits();

    for (std::size_t i = 0; i < j.size(); ++i) {
        const auto& jsonItem = j[i];
        if (jsonItem.empty() || !jsonItem.is_object())
            continue;

        if (!jsonItem.contains("Type") || !jsonItem["Type"].is_string())
            continue;

        const std::string type = jsonItem["Type"];
        if (type == "Sticker") {
            if (!jsonItem.contains("Sticker ID") || !jsonItem["Sticker ID"].is_number_integer())
                continue;

            const int stickerID = jsonItem["Sticker ID"];
            const auto staticData = std::ranges::find_if(std::as_const(gameItems), [stickerID](const auto& gameItem) { return gameItem.type == SkinChanger::GameItem::Type::Sticker && stickerData[gameItem.dataIndex].stickerID == stickerID; });
            if (staticData == gameItems.end())
                continue;

            // TODO: Add a mutex to guard inventory, lock it here
            inventory.emplace_back(std::ranges::distance(gameItems.begin(), staticData));
        } else if (type == "Skin") {
            if (!jsonItem.contains("Paint Kit") || !jsonItem["Paint Kit"].is_number_integer())
                continue;

            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const int paintKit = jsonItem["Paint Kit"];
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(std::as_const(gameItems), [paintKit, weaponID](const auto& gameItem) { return gameItem.type == SkinChanger::GameItem::Type::Skin && skinData[gameItem.dataIndex].paintKit == paintKit && skinData[gameItem.dataIndex].weaponId == weaponID; });
            if (staticData == gameItems.end())
                continue;

            inventory.emplace_back(std::ranges::distance(gameItems.begin(), staticData));
            
            // Load dynamic data now

            if (jsonItem.contains("Name Tag") && jsonItem["Name Tag"].is_string())
                dynamicSkinData[inventory.back().getDynamicDataIndex()].nameTag = jsonItem["Name Tag"];

            if (!jsonItem.contains("Stickers") || !jsonItem["Stickers"].is_array())
                continue;

            const auto& stickers = jsonItem["Stickers"];
            for (std::size_t k = 0; k < stickers.size(); ++k) {
                if (k >= std::tuple_size_v<decltype(DynamicSkinData::stickers)>)
                    break;

                const auto& sticker = stickers[k];
                if (!sticker.is_object())
                    continue;

                if (!sticker.contains("Sticker ID") || !sticker["Sticker ID"].is_number_integer())
                    continue;

                const int stickerID = sticker["Sticker ID"];
                if (stickerID == 0)
                    continue;

                dynamicSkinData[inventory.back().getDynamicDataIndex()].stickers[k].stickerID = stickerID;
                if (sticker.contains("Wear") && sticker["Wear"].is_number_float())
                    dynamicSkinData[inventory.back().getDynamicDataIndex()].stickers[k].wear = sticker["Wear"];
            }
        } else if (type == "Glove") {

        }
    }
}

void InventoryChanger::resetConfig() noexcept
{
    if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
        if (const auto baseTypeCache = localInventory->getItemBaseTypeCache()) {
            for (std::size_t i = 0; i < inventory.size(); ++i) {
                const auto view = memory->getInventoryItemByItemID(localInventory, BASE_ITEMID + i);
                if (!view)
                    continue;
                const auto econItem = memory->getSOCData(view);
                if (!econItem)
                    continue;

                removeItemFromInventory(localInventory, baseTypeCache, econItem);
            }

        }
    }

    inventory.clear();
    dynamicSkinData.clear();
}

void InventoryChanger::clearInventory() noexcept
{
    resetConfig();
}

const std::vector<SkinChanger::GameItem>& SkinChanger::getGameItems() noexcept
{
    initializeKits();
    return gameItems;
}

const SkinChanger::StickerData& SkinChanger::getStickerData(std::size_t index) noexcept
{
    return stickerData[index];
}

const SkinChanger::GloveData& SkinChanger::getGloveData(std::size_t index) noexcept
{
    return gloveData[index];
}

const SkinChanger::SkinData& SkinChanger::getSkinData(std::size_t index) noexcept
{
    return skinData[index];
}

const SkinChanger::MusicData& SkinChanger::getMusicData(std::size_t index) noexcept
{
    return musicData[index];
}

const SkinChanger::CollectibleData& SkinChanger::getCollectibleData(std::size_t index) noexcept
{
    return collectibleData[index];
}

struct Icon {
    Texture texture;
    int lastReferencedFrame = 0;
};

static std::unordered_map<std::string, Icon> iconTextures;

ImTextureID SkinChanger::getItemIconTexture(const std::string& iconpath) noexcept
{
    if (iconpath.empty())
        return 0;

    auto& icon = iconTextures[iconpath];
    if (!icon.texture.get()) {
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
    }
    icon.lastReferencedFrame = ImGui::GetFrameCount();
    return icon.texture.get();
}

void SkinChanger::clearItemIconTextures() noexcept
{
    iconTextures.clear();
}

void SkinChanger::clearUnusedItemIconTextures() noexcept
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

static int random(int min, int max) noexcept
{
    return rand() % (max - min + 1) + min;
}

static int get_new_animation(const uint32_t model, const int sequence) noexcept
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

    // Hashes for best performance.
    switch (model) {
    case fnv::hash("models/weapons/v_knife_butterfly.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
        default:
            return sequence + 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_falchion_advanced.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_FALCHION_IDLE1;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence - 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_push.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_DAGGERS_IDLE1;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
            return random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
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
    }
    case fnv::hash("models/weapons/v_knife_survival_bowie.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_BOWIE_IDLE1;
        default:
            return sequence - 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_ursus.mdl"):
    case fnv::hash("models/weapons/v_knife_skeleton.mdl"):
    case fnv::hash("models/weapons/v_knife_outdoor.mdl"):
    case fnv::hash("models/weapons/v_knife_cord.mdl"):
    case fnv::hash("models/weapons/v_knife_canis.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
        default:
            return sequence + 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_stiletto.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(12, 13);
        }
    }
    case fnv::hash("models/weapons/v_knife_widowmaker.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(14, 15);
        }
    }
    default:
        return sequence;
    }
}

void SkinChanger::fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept
{
    if (!localPlayer)
        return;

    if (!is_knife(viewModelWeapon->itemDefinitionIndex2()))
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 0);
    if (!itemView)
        return;
   
    if (const auto soc = memory->getSOCData(itemView); !soc || wasItemCreatedByOsiris(soc->itemID))
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(viewModelWeapon->itemDefinitionIndex2());
    if (!def)
        return;

    if (const auto model = def->getPlayerDisplayModel())
        sequence = get_new_animation(fnv::hashRuntime(model), sequence);
}

SkinChanger::GameItem::GameItem(Type type, int rarity, std::size_t dataIndex, std::wstring&& name, std::string&& iconPath) noexcept : type{ type }, rarity{ rarity }, dataIndex{ dataIndex }, nameUpperCase{ std::move(name) }, iconPath{ std::move(iconPath) }
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}
