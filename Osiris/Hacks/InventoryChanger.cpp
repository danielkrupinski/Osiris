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
#include "InventoryChanger.h"
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
#include "../SDK/PlayerResource.h"
#include "../SDK/Platform.h"
#include "../SDK/WeaponId.h"

#include "../Helpers.h"

constexpr auto isKnife(WeaponId id) noexcept
{
    return (id >= WeaponId::Bayonet && id <= WeaponId::SkeletonKnife) || id == WeaponId::KnifeT || id == WeaponId::Knife;
}

class StaticData {
public:
    enum class Type {
        Sticker,
        Glove,
        Skin,
        Music,
        Collectible,
        NameTag
    };

    struct GameItem {
        GameItem(Type type, int rarity, std::size_t dataIndex, std::wstring&& name, std::string&& iconPath) noexcept;

        bool isSticker() const noexcept { return type == Type::Sticker; }
        bool isSkin() const noexcept { return type == Type::Skin; }
        bool isGlove() const noexcept { return type == Type::Glove; }
        bool isMusic() const noexcept { return type == Type::Music; }
        bool isCollectible() const noexcept { return type == Type::Collectible; }
        bool isNameTag() const noexcept { return type == Type::NameTag; }

        Type type;
        int rarity;
        std::size_t dataIndex;

        std::string name;
        std::wstring nameUpperCase;
        std::string iconPath;
    };

    struct Sticker {
        Sticker(int stickerID) : stickerID{ stickerID } {}

        int stickerID;
    };

    struct Glove {
        Glove(int paintKit, WeaponId weaponId) : paintKit{ paintKit }, weaponId{ weaponId } {}

        int paintKit;
        WeaponId weaponId;
    };

    struct Skin {
        Skin(int paintKit, WeaponId weaponId) : paintKit{ paintKit }, weaponId{ weaponId } {}

        int paintKit;
        WeaponId weaponId;
    };

    struct Music {
        Music(int musicID) : musicID{ musicID } {}

        int musicID;
    };

    struct Collectible {
        Collectible(WeaponId weaponId, bool isOriginal) : weaponId{ weaponId }, isOriginal{ isOriginal } {}

        WeaponId weaponId;
        bool isOriginal;
    };

    static const auto& gameItems() noexcept { return instance()._gameItems; }
    static const auto& stickers() noexcept { return instance()._stickers; }
    static const auto& gloves() noexcept { return instance()._gloves; }
    static const auto& skins() noexcept { return instance()._skins; }
    static const auto& music() noexcept { return instance()._music; }
    static const auto& collectibles() noexcept { return instance()._collectibles; }
private:
    StaticData(const StaticData&) = delete;

    struct KitWeapon {
        KitWeapon(int paintKit, WeaponId weaponId, const char* iconPath) noexcept : paintKit{ paintKit }, weaponId{ weaponId }, iconPath{ iconPath } {}
        int paintKit;
        WeaponId weaponId;
        const char* iconPath;
    };

    static std::vector<KitWeapon> getKitsWeapons(const UtlMap<std::uint64_t, AlternateIconData>& alternateIcons) noexcept
    {
        std::vector<KitWeapon> kitsWeapons;
        kitsWeapons.reserve(alternateIcons.numElements);

        for (const auto& node : alternateIcons) {
            // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_schema.cpp#L325-L329
            if (const auto encoded = node.key; (encoded & 3) == 0)
                kitsWeapons.emplace_back(int((encoded & 0xFFFF) >> 2), WeaponId(encoded >> 16), node.value.simpleName.data());
        }
        std::ranges::sort(kitsWeapons, {}, &KitWeapon::paintKit);
        return kitsWeapons;
    }

    StaticData()
    {
        assert(memory && interfaces);

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        const auto kitsWeapons = getKitsWeapons(itemSchema->alternateIcons);

        std::unordered_map<WeaponId, std::wstring> weaponNames;
        for (const auto& kitWeapon : kitsWeapons) {
            if (weaponNames.contains(kitWeapon.weaponId))
                continue;

            if (const auto itemDef = itemSchema->getItemDefinitionInterface(kitWeapon.weaponId))
                weaponNames.emplace(kitWeapon.weaponId, interfaces->localize->findSafe(itemDef->getItemBaseName()));
        }

        _gameItems.reserve(itemSchema->paintKits.lastAlloc);
        for (const auto& node : itemSchema->paintKits) {
            const auto paintKit = node.value;

            if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
                continue;

            const auto isGlove = (paintKit->id >= 10000);
            for (auto it = std::ranges::lower_bound(kitsWeapons, paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {
                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                std::wstring name = weaponNames[it->weaponId];
                name += L" | ";
                name += interfaces->localize->findSafe(paintKit->itemName.data() + 1);
                if (isGlove) {
                    _gloves.emplace_back(paintKit->id, it->weaponId);
                    _gameItems.emplace_back(Type::Glove, paintKit->rarity, _gloves.size() - 1, std::move(name), it->iconPath);
                } else {
                    _skins.emplace_back(paintKit->id, it->weaponId);
                    _gameItems.emplace_back(Type::Skin, std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6), _skins.size() - 1, std::move(name), it->iconPath);
                }
            }
        }

        const auto& stickerMap = itemSchema->stickerKits;
        _gameItems.reserve(_gameItems.size() + stickerMap.numElements);
        for (const auto& node : stickerMap) {
            const auto stickerKit = node.value;
            if (stickerKit->id == 0)
                continue;

            if (std::string_view name{ stickerKit->name.data() }; name.starts_with("spray") || name.starts_with("patch") || name.ends_with("graffiti"))
                continue;

            std::wstring name = interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() + 1 : "StickerKit_dhw2014_teamdignitas_gold");
            _stickers.emplace_back(stickerKit->id);
            _gameItems.emplace_back(Type::Sticker, stickerKit->rarity, _stickers.size() - 1, std::move(name), stickerKit->inventoryImage.data());
        }

        const auto& musicMap = itemSchema->musicKits;
        for (const auto& node : musicMap) {
            const auto musicKit = node.value;
            if (musicKit->id == 1 || musicKit->id == 2)
                continue;

            _music.emplace_back(musicKit->id);
            _gameItems.emplace_back(Type::Music, 3, _music.size() - 1, interfaces->localize->findSafe(musicKit->nameLocalized), musicKit->inventoryImage);
        }

        for (const auto& node : itemSchema->itemsSorted) {
            const auto item = node.value;
            const auto itemTypeName = std::string_view{ item->getItemTypeName() };

            if (itemTypeName == "#CSGO_Type_Knife" && item->getRarity() == 6) {
                if (const auto image = item->getInventoryImage()) {
                    _skins.emplace_back(0, item->getWeaponId());
                    _gameItems.emplace_back(Type::Skin, 6, _skins.size() - 1, interfaces->localize->findSafe(item->getItemBaseName()), image);
                }
            } else if (itemTypeName == "#CSGO_Type_Collectible") {
                if (const auto image = item->getInventoryImage()) {
                    const auto isOriginal = (item->getQuality() == 1);
                    _collectibles.emplace_back(item->getWeaponId(), isOriginal);
                    std::wstring name = interfaces->localize->findSafe(item->getItemBaseName());
                    if (isOriginal) {
                        name += L" (";
                        name += interfaces->localize->findSafe("genuine");
                        name += L") ";
                    }
                    _gameItems.emplace_back(Type::Collectible, item->getRarity(), _collectibles.size() - 1, std::move(name), image);
                }
            } else if (itemTypeName == "#CSGO_Tool_Name_TagTag") {
                if (const auto image = item->getInventoryImage())
                    _gameItems.emplace_back(Type::NameTag, item->getRarity(), 0, interfaces->localize->findSafe(item->getItemBaseName()), image);
            }
        }

        std::ranges::sort(_gameItems, {}, &GameItem::nameUpperCase);
        _gameItems.shrink_to_fit();
    }

    static StaticData& instance() noexcept
    {
        static StaticData staticData;
        return staticData;
    }

    std::vector<GameItem> _gameItems;
    std::vector<Sticker> _stickers;
    std::vector<Glove> _gloves;
    std::vector<Skin> _skins;
    std::vector<Music> _music;
    std::vector<Collectible> _collectibles;
};

struct StickerConfig {
    int stickerID = 0;
    float wear = 0.0f;
};

struct DynamicSkinData {
    float wear = 0.0f;
    int seed = 1;
    std::array<StickerConfig, 5> stickers;
    std::string nameTag;
};

struct DynamicGloveData {
    float wear = 0.0f;
    int seed = 1;
};

static std::vector<DynamicSkinData> dynamicSkinData;
static std::vector<DynamicGloveData> dynamicGloveData;

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
public:
    InventoryItem(std::size_t itemIndex)  noexcept : itemIndex{ itemIndex }
    {
        if (isSkin()) {
            DynamicSkinData dynamicData;
            dynamicData.wear = randomFloat(0.0f, 0.07f);
            dynamicData.seed = randomInt(1, 1000);
            dynamicSkinData.push_back(dynamicData);
            dynamicDataIndex = dynamicSkinData.size() - 1;
        } else if (isGlove()) {
            DynamicGloveData dynamicData;
            dynamicData.wear = randomFloat(0.0f, 0.07f);
            dynamicData.seed = randomInt(1, 1000);
            dynamicGloveData.push_back(dynamicData);
            dynamicDataIndex = dynamicGloveData.size() - 1;
        }
    }

    void markAsDeleted() noexcept { itemIndex = static_cast<std::size_t>(-1); }
    bool isDeleted() const noexcept { return itemIndex == static_cast<std::size_t>(-1); }

    bool isSticker() const noexcept { return !isDeleted() && get().isSticker(); }
    bool isSkin() const noexcept { return !isDeleted() && get().isSkin(); }
    bool isGlove() const noexcept { return !isDeleted() && get().isGlove(); }
    bool isMusic() const noexcept { return !isDeleted() && get().isMusic(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(dynamicDataIndex != static_cast<std::size_t>(-1)); return dynamicDataIndex; }

    const StaticData::GameItem& get() const noexcept { assert(!isDeleted()); return StaticData::gameItems()[itemIndex]; }
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

    const auto& itemData = StaticData::gloves()[item.get().dataIndex];

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
    glove->entityQuality() = 3;
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

    const auto& itemData = StaticData::skins()[item.get().dataIndex];

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

        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
        weapon->entityQuality() = 3;

        const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

        constexpr auto m_Item = fnv::hash("CBaseAttributableItem->m_Item");
        const auto attributeList = std::uintptr_t(weapon) + netvars->operator[](m_Item) + /* m_AttributeList = */ WIN32_LINUX(0x244, 0x2F8);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.paintKit));
        memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", dynamicData.wear);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(dynamicData.seed));

        if (dynamicData.nameTag.length() < 32)
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
        if (isKnife(definitionIndex))
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

        const auto& itemData = StaticData::skins()[item.get().dataIndex];

        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);

        const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

        constexpr auto m_Item = fnv::hash("CBaseAttributableItem->m_Item");
        const auto attributeList = std::uintptr_t(weapon) + netvars->operator[](m_Item) + /* m_AttributeList = */ WIN32_LINUX(0x244, 0x2F8);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.paintKit));
        memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", dynamicData.wear);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(dynamicData.seed));

        if (dynamicData.nameTag.length() < 32)
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
void InventoryChanger::setStickerApplySlot(int slot) noexcept
{
    slotToApplySticker = slot;
}

static std::uint64_t toolToUse = 0;
static float useToolTime = 0.0f;
void InventoryChanger::setToolToUse(std::uint64_t itemID) noexcept
{
    toolToUse = itemID;

    std::mt19937 gen{ std::random_device{}() };
    std::uniform_real_distribution dis{ 0.3f, 0.7f };
    useToolTime = memory->globalVars->realtime + dis(gen);
}

static std::uint64_t itemToApplyTool = 0;
void InventoryChanger::setItemToApplyTool(std::uint64_t itemID) noexcept
{
    itemToApplyTool = itemID;
}

static int slotToWearSticker;
void InventoryChanger::setStickerSlotToWear(int slot) noexcept
{
    slotToWearSticker = slot;
}

static std::uint64_t itemToWearSticker = 0;
void InventoryChanger::setItemToWearSticker(std::uint64_t itemID) noexcept
{
    itemToWearSticker = itemID;
    useToolTime = memory->globalVars->realtime + randomFloat(0.18f, 0.48f);
}

static std::string nameTagString;
void InventoryChanger::setNameTagString(const char* str) noexcept
{
    nameTagString = str;
}

static std::uint64_t itemToRemoveNameTag = 0;
void InventoryChanger::setItemToRemoveNameTag(std::uint64_t itemID) noexcept
{
    itemToRemoveNameTag = itemID;
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

static void removeItemFromInventory(CSPlayerInventory* inventory, SharedObjectTypeCache<EconItem>* cache, EconItem* econItem) noexcept
{
    inventory->removeItem(econItem->itemID);
    cache->removeObject(econItem);
}

struct ToEquip {
    ToEquip(Team team, int slot, std::size_t index) : team{ team }, slot{ slot }, index{ index } {}

    Team team;
    int slot;
    std::size_t index;
};

static std::vector<ToEquip> toEquip;

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
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    const auto& item = inventory[static_cast<std::size_t>(soc->itemID - BASE_ITEMID)];
    if (!item.isMusic())
        return;

    const auto& itemData = StaticData::music()[item.get().dataIndex];
    pr->musicID()[localPlayer->index()] = itemData.musicID;
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

    static const auto baseInvID = localInventory->getHighestIDs().second;

    std::uint64_t appliedStickerToItemID = 0;
    std::uint64_t addedNameTagToItemID = 0;
    if (useToolTime <= memory->globalVars->realtime) {
        if (wasItemCreatedByOsiris(toolToUse) && wasItemCreatedByOsiris(itemToApplyTool)) {
            auto& tool = inventory[static_cast<std::size_t>(toolToUse - BASE_ITEMID)];
            const auto& toolItem = tool.get();
            auto& dest = inventory[static_cast<std::size_t>(itemToApplyTool - BASE_ITEMID)];

            if (dest.isSkin() && (toolItem.isSticker() || toolItem.isNameTag())) {
                if (const auto view = memory->findOrCreateEconItemViewForItemID(itemToApplyTool)) {
                    if (toolItem.isSticker()) {
                        const auto& sticker = StaticData::stickers()[toolItem.dataIndex];
                        dynamicSkinData[dest.getDynamicDataIndex()].stickers[slotToApplySticker].stickerID = sticker.stickerID;
                        dynamicSkinData[dest.getDynamicDataIndex()].stickers[slotToApplySticker].wear = 0.0f;
                        appliedStickerToItemID = BASE_ITEMID + inventory.size();
                    } else if (toolItem.isNameTag()) {
                        dynamicSkinData[dest.getDynamicDataIndex()].nameTag = nameTagString;
                        addedNameTagToItemID = BASE_ITEMID + inventory.size();
                    }

                    if (const auto toolView = memory->findOrCreateEconItemViewForItemID(toolToUse))
                        if (const auto econItem = memory->getSOCData(toolView))
                            removeItemFromInventory(localInventory, baseTypeCache, econItem);

                    if (const auto econItem = memory->getSOCData(view)) {
                        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(econItem->weaponId)) {
                            if (const auto slotCT = def->getLoadoutSlot(Team::CT); localInventory->getItemInLoadout(Team::CT, slotCT) == view)
                                toEquip.emplace_back(Team::CT, slotCT, inventory.size());
                            if (const auto slotTT = def->getLoadoutSlot(Team::TT); localInventory->getItemInLoadout(Team::TT, slotTT) == view)
                                toEquip.emplace_back(Team::TT, slotTT, inventory.size());
                        }
                        removeItemFromInventory(localInventory, baseTypeCache, econItem);
                    }
                    tool.markAsDeleted();
                    auto destCopy = dest;
                    dest.markAsDeleted();
                    inventory.push_back(std::move(destCopy));
                }
            }
        } else if (wasItemCreatedByOsiris(itemToWearSticker)) {
            constexpr auto wearStep = 0.12f;

            const auto& item = inventory[static_cast<std::size_t>(itemToWearSticker - BASE_ITEMID)];
            const auto newWear = (dynamicSkinData[item.getDynamicDataIndex()].stickers[slotToWearSticker].wear += 0.12f);
            const auto shouldRemove = (newWear >= 1.0f + wearStep);

            if (shouldRemove)
                dynamicSkinData[item.getDynamicDataIndex()].stickers[slotToWearSticker] = {};

            if (const auto view = memory->findOrCreateEconItemViewForItemID(itemToWearSticker)) {
                if (const auto soc = memory->getSOCData(view)) {
                    if (shouldRemove) {
                        soc->setStickerID(slotToWearSticker, 0);
                        soc->setStickerWear(slotToWearSticker, 0.0f);
                    } else {
                        soc->setStickerWear(slotToWearSticker, newWear);
                    }
                }
            }

            sendInventoryUpdatedEvent();

            if (shouldRemove)
                initItemCustomizationNotification("sticker_remove", std::to_string(itemToWearSticker).c_str());
        } else if (wasItemCreatedByOsiris(itemToRemoveNameTag)) {
            if (const auto view = memory->findOrCreateEconItemViewForItemID(itemToRemoveNameTag)) {
                auto& item = inventory[static_cast<std::size_t>(itemToRemoveNameTag - BASE_ITEMID)];
                dynamicSkinData[item.getDynamicDataIndex()].nameTag.clear();

                if (const auto econItem = memory->getSOCData(view)) {
                    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(econItem->weaponId)) {
                        if (const auto slotCT = def->getLoadoutSlot(Team::CT); localInventory->getItemInLoadout(Team::CT, slotCT) == view)
                            toEquip.emplace_back(Team::CT, slotCT, inventory.size());
                        if (const auto slotTT = def->getLoadoutSlot(Team::TT); localInventory->getItemInLoadout(Team::TT, slotTT) == view)
                            toEquip.emplace_back(Team::TT, slotTT, inventory.size());
                    }
                    removeItemFromInventory(localInventory, baseTypeCache, econItem);
                }

                auto itemCopy = item;
                item.markAsDeleted();
                inventory.push_back(std::move(itemCopy));
            }
        }
        itemToRemoveNameTag = itemToWearSticker = toolToUse = itemToApplyTool = 0;
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
        econItem->quality = 4;

        if (item.isSticker()) {
            econItem->weaponId = WeaponId::Sticker;
            econItem->setStickerID(0, StaticData::stickers()[item.dataIndex].stickerID);
        } else if (item.isMusic()) {
            econItem->weaponId = WeaponId::MusicKit;
            econItem->setMusicID(StaticData::music()[item.dataIndex].musicID);
        } else if (item.isSkin()) {
            econItem->weaponId = StaticData::skins()[item.dataIndex].weaponId;
            if (isKnife(econItem->weaponId))
                econItem->quality = 3;
            econItem->setPaintKit(static_cast<float>(StaticData::skins()[item.dataIndex].paintKit));

            const auto& dynamicData = dynamicSkinData[inventory[i].getDynamicDataIndex()];
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
            econItem->weaponId = StaticData::gloves()[item.dataIndex].weaponId;
            econItem->quality = 3;

            econItem->setPaintKit(static_cast<float>(StaticData::gloves()[item.dataIndex].paintKit));

            const auto& dynamicData = dynamicGloveData[inventory[i].getDynamicDataIndex()];
            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
        } else if (item.isCollectible()) {
            econItem->weaponId = StaticData::collectibles()[item.dataIndex].weaponId;
            if (StaticData::collectibles()[item.dataIndex].isOriginal)
                econItem->quality = 1;
        } else if (item.isNameTag()) {
            econItem->weaponId = WeaponId::NameTag;
        }

        baseTypeCache->addObject(econItem);
        memory->addEconItem(localInventory, econItem, false, false, false);

        if (const auto view = memory->findOrCreateEconItemViewForItemID(econItem->itemID))
            memory->clearInventoryImageRGBA(view);
    }

    for (const auto& item : toEquip)
        memory->inventoryManager->equipItemInSlot(item.team, item.slot, item.index + BASE_ITEMID);

    toEquip.clear();

    if (appliedStickerToItemID)
        initItemCustomizationNotification("sticker_apply", std::to_string(appliedStickerToItemID).c_str());

    if (addedNameTagToItemID)
        initItemCustomizationNotification("nametag_add", std::to_string(addedNameTagToItemID).c_str());
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
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(soc->weaponId)) {
        if (const auto defName = def->getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
            event.setString("weapon", defName + 7);
    }
}

void InventoryChanger::updateStatTrak(GameEvent& event) noexcept
{
    constexpr auto implemented = false;
    if constexpr (!implemented)
        return;

    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    /*
    if (const auto conf = get_by_definition_index(isKnife(weapon->itemDefinitionIndex2()) ? WeaponId::Knife : weapon->itemDefinitionIndex2()); conf && conf->stat_trak > -1) {
        weapon->fallbackStatTrak() = ++conf->stat_trak;
        weapon->postDataUpdate(0);
    }
    */
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
    static bool SkinSelectable(const StaticData::GameItem& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool selected = false) noexcept
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const char* label = item.name.c_str();
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

        if (const auto icon = getItemIconTexture(item.iconPath)) {
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
        static std::unordered_set<std::size_t> selectedToAdd;
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
                if (!filter.empty() && !passesFilter(gameItems[i].nameUpperCase, filterWide))
                    continue;
                ImGui::PushID(i);
                const auto selected = selectedToAdd.contains(i);
                if (ImGui::SkinSelectable(gameItems[i], { 35.0f, 26.25f }, { 200.0f, 150.0f }, rarityColor(gameItems[i].rarity), selected)) {
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
        /*
        for (std::size_t i = 0; i < inventory.size(); ++i) {
            if (inventory[i].isDeleted())
                continue;

            ImGui::PushID(i);
            ImGui::Image(getItemIconTexture(inventory[i].get().iconPath), { 100.0f, 75.0f });
            ImGui::PopID();
        }
        */
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
            const auto& staticData = StaticData::stickers()[gameItem.dataIndex];
            itemConfig["Sticker ID"] = staticData.stickerID;
            break;
        }
        case StaticData::Type::Glove: {
            itemConfig["Type"] = "Glove";
            const auto& staticData = StaticData::gloves()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.paintKit;
            itemConfig["Weapon ID"] = staticData.weaponId;

            const auto& dynamicData = dynamicGloveData[item.getDynamicDataIndex()];

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
            break;
        }
        case StaticData::Type::Skin: {
            itemConfig["Type"] = "Skin";
            const auto& staticData = StaticData::skins()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.paintKit;
            itemConfig["Weapon ID"] = staticData.weaponId;

            const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;

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
            const auto& staticData = StaticData::music()[gameItem.dataIndex];
            itemConfig["Music ID"] = staticData.musicID;
            break;
        }
        case StaticData::Type::Collectible: {
            itemConfig["Type"] = "Collectible";
            const auto& staticData = StaticData::collectibles()[gameItem.dataIndex];
            itemConfig["Weapon ID"] = staticData.weaponId;
            itemConfig["Is Original"] = staticData.isOriginal;
            break;
        }
        case StaticData::Type::NameTag: {
            itemConfig["Type"] = "Name Tag";
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
                if (const auto soc = memory->getSOCData(itemCT); soc && wasItemCreatedByOsiris(soc->itemID))
                    slot["CT"] = static_cast<std::size_t>(soc->itemID - BASE_ITEMID - std::count_if(inventory.begin(), inventory.begin() + static_cast<std::size_t>(soc->itemID - BASE_ITEMID), [](const auto& item) { return item.isDeleted(); }));
            }

            if (const auto itemTT = localInventory->getItemInLoadout(Team::TT, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemTT); soc && wasItemCreatedByOsiris(soc->itemID))
                    slot["TT"] = static_cast<std::size_t>(soc->itemID - BASE_ITEMID - std::count_if(inventory.begin(), inventory.begin() + static_cast<std::size_t>(soc->itemID - BASE_ITEMID), [](const auto& item) { return item.isDeleted(); }));
            }

            if (const auto itemNOTEAM = localInventory->getItemInLoadout(Team::None, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemNOTEAM); soc && wasItemCreatedByOsiris(soc->itemID))
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
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [stickerID](const auto& gameItem) { return gameItem.isSticker() && StaticData::stickers()[gameItem.dataIndex].stickerID == stickerID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData));
        } else if (type == "Skin") {
            if (!jsonItem.contains("Paint Kit") || !jsonItem["Paint Kit"].is_number_integer())
                continue;

            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const int paintKit = jsonItem["Paint Kit"];
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [paintKit, weaponID](const auto& gameItem) { return gameItem.isSkin() && StaticData::skins()[gameItem.dataIndex].paintKit == paintKit && StaticData::skins()[gameItem.dataIndex].weaponId == weaponID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData));

            auto& dynamicData = dynamicSkinData[inventory.back().getDynamicDataIndex()];

            if (jsonItem.contains("Wear")) {
                if (const auto& wear = jsonItem["Wear"]; wear.is_number_float())
                    dynamicData.wear = wear;
            }

            if (jsonItem.contains("Seed")) {
                if (const auto& seed = jsonItem["Seed"]; seed.is_number_integer())
                    dynamicData.seed = seed;
            }

            if (jsonItem.contains("Name Tag")) {
                if (const auto& nameTag = jsonItem["Name Tag"]; nameTag.is_string())
                    dynamicData.nameTag = nameTag;
            }

            if (!jsonItem.contains("Stickers"))
                continue;

            const auto& stickers = jsonItem["Stickers"];
            if (!stickers.is_array())
                continue;
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
        } else if (type == "Glove") {
            if (!jsonItem.contains("Paint Kit") || !jsonItem["Paint Kit"].is_number_integer())
                continue;

            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;

            const int paintKit = jsonItem["Paint Kit"];
            const WeaponId weaponID = jsonItem["Weapon ID"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [paintKit, weaponID](const auto& gameItem) { return gameItem.isGlove() && StaticData::gloves()[gameItem.dataIndex].paintKit == paintKit && StaticData::gloves()[gameItem.dataIndex].weaponId == weaponID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData));

            auto& dynamicData = dynamicGloveData[inventory.back().getDynamicDataIndex()];

            if (jsonItem.contains("Wear")) {
                if (const auto& wear = jsonItem["Wear"]; wear.is_number_float())
                    dynamicData.wear = wear;
            }

            if (jsonItem.contains("Seed")) {
                if (const auto& seed = jsonItem["Seed"]; seed.is_number_integer())
                    dynamicData.seed = seed;
            }
        } else if (type == "Music") {
            if (!jsonItem.contains("Music ID") || !jsonItem["Music ID"].is_number_integer())
                continue;

            const int musicID = jsonItem["Music ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [musicID](const auto& gameItem) { return gameItem.isMusic() && StaticData::music()[gameItem.dataIndex].musicID == musicID; });

            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData));
        } else if (type == "Collectible") {
            if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
                continue;
            if (!jsonItem.contains("Is Original") || !jsonItem["Is Original"].is_boolean())
                continue;

            const WeaponId weaponID = jsonItem["Weapon ID"];
            const bool isOriginal = jsonItem["Is Original"];

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [weaponID, isOriginal](const auto& gameItem) { return gameItem.isCollectible() && StaticData::collectibles()[gameItem.dataIndex].weaponId == weaponID && StaticData::collectibles()[gameItem.dataIndex].isOriginal == isOriginal; });

            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData));
        } else if (type == "Name Tag") {
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [](const auto& gameItem) { return gameItem.isNameTag(); });
            if (staticData != StaticData::gameItems().end())
                inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData));
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
            if (const auto& ct = equipped["CT"]; ct.is_number_integer() && ct < inventory.size())
                toEquip.emplace_back(Team::CT, slot, ct);
        }

        if (equipped.contains("TT")) {
            if (const auto& tt = equipped["TT"]; tt.is_number_integer() && tt < inventory.size())
                toEquip.emplace_back(Team::TT, slot, tt);
        }

        if (equipped.contains("NOTEAM")) {
            if (const auto& noteam = equipped["NOTEAM"]; noteam.is_number_integer() && noteam < inventory.size())
                toEquip.emplace_back(Team::None, slot, noteam);
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

    sendInventoryUpdatedEvent();
}

void InventoryChanger::clearInventory() noexcept
{
    resetConfig();
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

static int random(int min, int max) noexcept
{
    return rand() % (max - min + 1) + min;
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
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
        default:
            return sequence + 1;
        }
    case WeaponId::Falchion:
        switch (sequence) {
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
    case WeaponId::Daggers:
        switch (sequence) {
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
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
        default:
            return sequence + 1;
        }
    case WeaponId::Stiletto:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(12, 13);
        }
    case WeaponId::Talon:
        switch (sequence) {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(14, 15);
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

    if (const auto soc = memory->getSOCData(itemView); !soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    sequence = remapKnifeAnim(viewModelWeapon->itemDefinitionIndex2(), sequence);
}

StaticData::GameItem::GameItem(Type type, int rarity, std::size_t dataIndex, std::wstring&& name, std::string&& iconPath) noexcept : type{ type }, rarity{ rarity }, dataIndex{ dataIndex }, nameUpperCase{ std::move(name) }, iconPath{ std::move(iconPath) }
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}
