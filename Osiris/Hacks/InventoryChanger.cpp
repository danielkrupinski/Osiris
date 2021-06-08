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
#include "../SDK/NetworkStringTable.h"

#include "../Helpers.h"

constexpr auto isKnife(WeaponId id) noexcept
{
    return (id >= WeaponId::Bayonet && id <= WeaponId::SkeletonKnife) || id == WeaponId::KnifeT || id == WeaponId::Knife;
}

constexpr auto isGlove(WeaponId id) noexcept
{
    switch (id)
    {
    case WeaponId::GloveStuddedBrokenfang:
    case WeaponId::GloveStuddedBloodhound:
    case WeaponId::GloveT:
    case WeaponId::GloveCT:
    case WeaponId::GloveSporty:
    case WeaponId::GloveSlick:
    case WeaponId::GloveLeatherWrap:
    case WeaponId::GloveMotorcycle:
    case WeaponId::GloveSpecialist:
    case WeaponId::GloveHydra:
        return true;
    default:
        return false;
    }
}

class StaticData {
public:
    enum class Type : std::uint8_t {
        Sticker,
        Glove,
        Skin,
        Music,
        Collectible,
        NameTag,
        Agent,
        Patch,
        Graffiti,
        SealedGraffiti
    };

    struct GameItem {
        GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath) noexcept;

        bool isSticker() const noexcept { return type == Type::Sticker; }
        bool isSkin() const noexcept { return type == Type::Skin; }
        bool isGlove() const noexcept { return type == Type::Glove; }
        bool isMusic() const noexcept { return type == Type::Music; }
        bool isCollectible() const noexcept { return type == Type::Collectible; }
        bool isNameTag() const noexcept { return type == Type::NameTag; }
        bool isAgent() const noexcept { return type == Type::Agent; }
        bool isPatch() const noexcept { return type == Type::Patch; }
        bool isGraffiti() const noexcept { return type == Type::Graffiti; }
        bool isSealedGraffiti() const noexcept { return type == Type::SealedGraffiti; }

        // TODO: We need a better name for this
        bool hasPaintKit() const noexcept { return isSkin() || isGlove() || isSticker() || isMusic() || isPatch() || isGraffiti() || isSealedGraffiti(); }

        Type type;
        std::uint8_t rarity;
        WeaponId weaponID;
        std::size_t dataIndex;

        std::string iconPath;
    };

    struct Collectible {
        Collectible(bool isOriginal) : isOriginal{ isOriginal } {}

        bool isOriginal;
    };

    struct PaintKit {
        PaintKit(int id, std::wstring&& name) noexcept;

        int id;
        std::string name;
        std::wstring nameUpperCase;
    };

    static const auto& gameItems() noexcept { return instance()._gameItems; }
    static const auto& collectibles() noexcept { return instance()._collectibles; }
    static const auto& paintKits() noexcept { return instance()._paintKits; }
    static const auto& getWeaponNameUpper(WeaponId weaponID) noexcept { return instance()._weaponNamesUpper[weaponID]; }
    static const auto& getWeaponName(WeaponId weaponID) noexcept { return instance()._weaponNames[weaponID]; }
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

        _gameItems.reserve(itemSchema->paintKits.lastAlloc);
        for (const auto& node : itemSchema->paintKits) {
            const auto paintKit = node.value;

            if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
                continue;

            _paintKits.emplace_back(paintKit->id, interfaces->localize->findSafe(paintKit->itemName.data()));

            const auto isGlove = (paintKit->id >= 10000);
            for (auto it = std::ranges::lower_bound(kitsWeapons, paintKit->id, {}, &KitWeapon::paintKit); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {
                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                if (isGlove) {
                    _gameItems.emplace_back(Type::Glove, paintKit->rarity, it->weaponId, _paintKits.size() - 1, it->iconPath);
                } else {
                    _gameItems.emplace_back(Type::Skin, std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6), it->weaponId, _paintKits.size() - 1, it->iconPath);
                }
            }
        }

        const auto& stickerMap = itemSchema->stickerKits;
        _gameItems.reserve(_gameItems.size() + stickerMap.numElements);
        for (const auto& node : stickerMap) {
            const auto stickerKit = node.value;
            if (stickerKit->id == 0)
                continue;

            const auto name = std::string_view{ stickerKit->name.data() };
            const auto isPatch = name.starts_with("patch");
            const auto isGraffiti = !isPatch && (name.starts_with("spray") || name.ends_with("graffiti"));
            const auto isSticker = !isPatch && !isGraffiti;

            if (isSticker) {
                _paintKits.emplace_back(stickerKit->id, interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() : "StickerKit_dhw2014_teamdignitas_gold"));
                _gameItems.emplace_back(Type::Sticker, stickerKit->rarity, WeaponId::Sticker, _paintKits.size() - 1, stickerKit->inventoryImage.data());
            } else if (isPatch) {
                _paintKits.emplace_back(stickerKit->id, interfaces->localize->findSafe(stickerKit->itemName.data()));
                _gameItems.emplace_back(Type::Patch, stickerKit->rarity, WeaponId::Patch, _paintKits.size() - 1, stickerKit->inventoryImage.data());
            } else if (isGraffiti) {
                _paintKits.emplace_back(stickerKit->id, interfaces->localize->findSafe(stickerKit->itemName.data()));
                _gameItems.emplace_back(Type::Graffiti, stickerKit->rarity, WeaponId::Graffiti, _paintKits.size() - 1, stickerKit->inventoryImage.data());
                _gameItems.emplace_back(Type::SealedGraffiti, stickerKit->rarity, WeaponId::SealedGraffiti, _paintKits.size() - 1, stickerKit->inventoryImage.data());
            }
        }

        const auto& musicMap = itemSchema->musicKits;
        for (const auto& node : musicMap) {
            const auto musicKit = node.value;
            if (musicKit->id == 1 || musicKit->id == 2)
                continue;

            std::wstring name = interfaces->localize->findSafe(musicKit->nameLocalized);
            _paintKits.emplace_back(musicKit->id, std::move(name));
            _gameItems.emplace_back(Type::Music, 3, WeaponId::MusicKit, _paintKits.size() - 1, musicKit->inventoryImage);
        }

        for (const auto& node : itemSchema->itemsSorted) {
            const auto item = node.value;
            const auto itemTypeName = std::string_view{ item->getItemTypeName() };
            const auto isCollectible = (itemTypeName == "#CSGO_Type_Collectible");
            const auto isOriginal = (item->getQuality() == 1);

            if (!_weaponNames.contains(item->getWeaponId())) {
                std::wstring nameWide = interfaces->localize->findSafe(item->getItemBaseName());
                if (isCollectible && isOriginal) {
                    nameWide += L" (";
                    nameWide += interfaces->localize->findSafe("genuine");
                    nameWide += L") ";
                }
                _weaponNames.emplace(item->getWeaponId(), interfaces->localize->convertUnicodeToAnsi(nameWide.c_str()));
                _weaponNamesUpper.emplace(item->getWeaponId(), Helpers::toUpper(nameWide));
            }

            if (itemTypeName == "#CSGO_Type_Knife" && item->getRarity() == 6) {
                if (const auto image = item->getInventoryImage()) {
                    _paintKits.emplace_back(0, L"");
                    _gameItems.emplace_back(Type::Skin, 6, item->getWeaponId(), _paintKits.size() - 1, image);
                }
            } else if (isCollectible) {
                if (const auto image = item->getInventoryImage()) {
                    _collectibles.emplace_back(isOriginal);
                    _gameItems.emplace_back(Type::Collectible, item->getRarity(), item->getWeaponId(), _collectibles.size() - 1, image);
                }
            } else if (item->getCapabilities() & ITEM_CAP_CAN_PATCH) {
                if (const auto image = item->getInventoryImage())
                    _gameItems.emplace_back(Type::Agent, item->getRarity(), item->getWeaponId(), 0, image);
            } else if (itemTypeName == "#CSGO_Tool_Name_TagTag") {
                if (const auto image = item->getInventoryImage())
                    _gameItems.emplace_back(Type::NameTag, item->getRarity(), item->getWeaponId(), 0, image);
            }
        }

        std::ranges::sort(_gameItems, [this](const auto& a, const auto& b) {
            if (_weaponNamesUpper[a.weaponID] < _weaponNamesUpper[b.weaponID])
                return true;
            if (a.hasPaintKit() && b.hasPaintKit() && _weaponNamesUpper[a.weaponID] == _weaponNamesUpper[b.weaponID])
                return _paintKits[a.dataIndex].nameUpperCase < _paintKits[b.dataIndex].nameUpperCase;

            return false;
        });
        _gameItems.shrink_to_fit();
    }

    static StaticData& instance() noexcept
    {
        static StaticData staticData;
        return staticData;
    }

    std::vector<GameItem> _gameItems;
    std::vector<Collectible> _collectibles;
    std::vector<PaintKit> _paintKits;
    std::unordered_map<WeaponId, std::string> _weaponNames;
    std::unordered_map<WeaponId, std::wstring> _weaponNamesUpper;
};

struct StickerConfig {
    int stickerID = 0;
    float wear = 0.0f;
};

struct DynamicSkinData {
    float wear = 0.0f;
    int seed = 1;
    int statTrak = -1;
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
    bool apply = false;
public:
    InventoryItem(std::size_t itemIndex, bool apply2 = false)  noexcept : itemIndex{ itemIndex }, apply{ apply2 }
    {
        if (apply2)
            markAsApply();

        if (isSkin())
        {
            DynamicSkinData dynamicData;
            dynamicSkinData.push_back(dynamicData);
            dynamicDataIndex = dynamicSkinData.size() - 1;
        }
        else if (isGlove())
        {
            DynamicGloveData dynamicData;
            dynamicGloveData.push_back(dynamicData);
            dynamicDataIndex = dynamicGloveData.size() - 1;
        }
    }

    void markAsDeleted() noexcept { itemIndex = static_cast<std::size_t>(-1); }
    bool isDeleted() const noexcept { return itemIndex == static_cast<std::size_t>(-1); }

    void markAsApply() noexcept { apply = true; }
    bool isApply() const noexcept { return apply; }

    bool isSticker() const noexcept { return !isDeleted() && get().isSticker(); }
    bool isSkin() const noexcept { return !isDeleted() && get().isSkin(); }
    bool isGlove() const noexcept { return !isDeleted() && get().isGlove(); }
    bool isMusic() const noexcept { return !isDeleted() && get().isMusic(); }
    bool isCollectible() const noexcept { return !isDeleted() && get().isCollectible(); }
    bool isNameTag() const noexcept { return !isDeleted() && get().isNameTag(); }
    bool isAgent() const noexcept { return !isDeleted() && get().isAgent(); }
    bool isPatch() const noexcept { return !isDeleted() && get().isPatch(); }
    bool isGraffiti() const noexcept { return !isDeleted() && get().isGraffiti(); }
    bool isSealedGraffiti() const noexcept { return !isDeleted() && get().isSealedGraffiti(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(dynamicDataIndex != static_cast<std::size_t>(-1)); return dynamicDataIndex; }

    const StaticData::GameItem& get() const noexcept { assert(!isDeleted()); return StaticData::gameItems()[itemIndex]; }
};

static std::vector<InventoryItem> inventory;

static bool wasItemCreatedByOsiris(std::uint64_t itemID) noexcept
{
    return itemID >= BASE_ITEMID && static_cast<std::size_t>(itemID - BASE_ITEMID) < inventory.size();
}

struct addOptions
{
    std::size_t idx;
    float wear = 0.0f;
    int seed = 0;
    bool statTrak = false;
    int statTrak_value = -1;
    std::array<StickerConfig, 5> stickers;
    int count = 1;
    bool sealed = false;
};

static void addToInventory(const std::vector<addOptions>& indicesToAdd) noexcept
{
    for (const auto option : indicesToAdd) {
        inventory.emplace_back(option.idx);

        if (inventory.back().isSkin())
        {
            auto& data = dynamicSkinData[inventory.back().getDynamicDataIndex()];
            data.wear = option.wear;
            data.seed = option.seed;
            data.statTrak = option.statTrak ? 0 : -1;
            data.stickers = option.stickers;
        }
        else if (inventory.back().isGlove())
        {
            auto& data = dynamicGloveData[inventory.back().getDynamicDataIndex()];
            data.wear = option.wear;
            data.seed = option.seed;
        }
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

    const auto& itemData = StaticData::paintKits()[item.get().dataIndex];

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

    const auto attributeList = glove->econItemView().getAttributeList();
    const auto& dynamicData = dynamicGloveData[item.getDynamicDataIndex()];
    memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.id));
    memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", dynamicData.wear);
    memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(dynamicData.seed));

    if (auto& definitionIndex = glove->itemDefinitionIndex2(); definitionIndex != item.get().weaponID) {
        definitionIndex = item.get().weaponID;

        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item.get().weaponID)) {
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

    const auto& itemData = StaticData::paintKits()[item.get().dataIndex];

    auto& weapons = local->weapons();

    PlayerInfo player_info;
    if (!interfaces->engine->getPlayerInfo(local->index(), player_info))
        return;
    
    for (auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;

        if (player_info.xuidLow != weapon->xuidLow() || player_info.xuidHigh != weapon->xuidHigh())
            continue;
        
        auto& definitionIndex = weapon->itemDefinitionIndex2();
        if (!isKnife(definitionIndex))
            continue;

        weapon->accountID() = player_info.xuidLow;
        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);
        weapon->entityQuality() = 3;

        const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

        const auto attributeList = weapon->econItemView().getAttributeList();
        memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.id));
        memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", dynamicData.wear);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(dynamicData.seed));

        if (dynamicData.statTrak > -1)
        {
            memory->setOrAddAttributeValueByName(attributeList, "kill eater score type", 0);
            memory->setOrAddAttributeValueByName(attributeList, "kill eater", dynamicData.statTrak);
            weapon->fallbackStatTrak() = dynamicData.statTrak;
        }

        if (dynamicData.nameTag.length() < 32)
            std::strncpy(weapon->customName(), dynamicData.nameTag.c_str(), 32);

        if (definitionIndex != item.get().weaponID) {
            definitionIndex = item.get().weaponID;

            if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item.get().weaponID)) {
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

    PlayerInfo player_info;
    if (!interfaces->engine->getPlayerInfo(local->index(), player_info))
        return;
    
    auto& weapons = local->weapons();
    for (auto weaponHandle : weapons) {
        if (weaponHandle == -1)
            break;

        const auto weapon = interfaces->entityList->getEntityFromHandle(weaponHandle);
        if (!weapon)
            continue;
        
        if (player_info.xuidLow != weapon->xuidLow() || player_info.xuidHigh != weapon->xuidHigh())
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

        const auto& itemData = StaticData::paintKits()[item.get().dataIndex];

        weapon->accountID() = player_info.xuidLow;
        weapon->itemIDHigh() = std::uint32_t(soc->itemID >> 32);
        weapon->itemIDLow() = std::uint32_t(soc->itemID & 0xFFFFFFFF);

        const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

        const auto attributeList = weapon->econItemView().getAttributeList();
        memory->setOrAddAttributeValueByName(attributeList, "set item texture prefab", static_cast<float>(itemData.id));
        memory->setOrAddAttributeValueByName(attributeList, "set item texture wear", dynamicData.wear);
        memory->setOrAddAttributeValueByName(attributeList, "set item texture seed", static_cast<float>(dynamicData.seed));

        if (dynamicData.statTrak > -1)
        {
            memory->setOrAddAttributeValueByName(attributeList, "kill eater score type", 0);
            memory->setOrAddAttributeValueByName(attributeList, "kill eater", dynamicData.statTrak);
            weapon->fallbackStatTrak() = dynamicData.statTrak;
        }

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
    useToolTime = memory->globalVars->realtime + randomFloat(0.3f, 0.7f);
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

    const auto& itemData = StaticData::paintKits()[item.get().dataIndex];
    pr->m_nMusicID()[localPlayer->index()] = itemData.id;
}

static void applyCollectibles(CSPlayerInventory& localInventory) noexcept
{
    if (!localPlayer)
        return;

    const auto pr = *memory->playerResource;
    if (pr == nullptr)
        return;

    const auto itemView = localInventory.getItemInLoadout(Team::None, 55);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    const auto& item = inventory[static_cast<std::size_t>(soc->itemID - BASE_ITEMID)];
    if (!item.isCollectible())
        return;

    pr->m_nActiveCoinRank()[localPlayer->index()] = (int)item.get().weaponID;
}

static void playerModel(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    static int originalIdx = 0;

    if (!localPlayer) {
        originalIdx = 0;
        return;
    }

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 38);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    const auto& item = inventory[static_cast<std::size_t>(soc->itemID - BASE_ITEMID)];
    if (!item.isAgent())
        return;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(item.get().weaponID))
    {
        if (const auto model = def->getPlayerDisplayModel()) {
            if (stage == FrameStage::RENDER_START) {
                originalIdx = localPlayer->modelIndex();
                if (const auto modelprecache = interfaces->networkStringTableContainer->findTable("modelprecache")) {
                    modelprecache->addString(false, model);
                    const auto viewmodelArmConfig = memory->getPlayerViewmodelArmConfigForPlayerModel(model);
                    modelprecache->addString(false, viewmodelArmConfig[2]);
                    modelprecache->addString(false, viewmodelArmConfig[3]);
                }
            }

            const auto idx = stage == FrameStage::RENDER_END && originalIdx ? originalIdx : interfaces->modelInfo->getModelIndex(model);

            localPlayer->setModelIndex(idx);

            if (const auto ragdoll = interfaces->entityList->getEntityFromHandle(localPlayer->ragdoll()))
                ragdoll->setModelIndex(idx);
        }
    }
}

void InventoryChanger::run(FrameStage stage) noexcept
{
    playerModel(stage);
    
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
    applyCollectibles(*localInventory);

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
                        const auto& sticker = StaticData::paintKits()[toolItem.dataIndex];
                        dynamicSkinData[dest.getDynamicDataIndex()].stickers[slotToApplySticker].stickerID = sticker.id;
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
            const auto newWear = (dynamicSkinData[item.getDynamicDataIndex()].stickers[slotToWearSticker].wear += wearStep);
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
        econItem->inventory = inventory[i].isApply() ? baseInvID + i + 1 : 0;
        econItem->rarity = item.rarity;
        econItem->quality = 4;
        econItem->weaponId = item.weaponID;

        if (item.isSticker() || item.isPatch() || item.isGraffiti() || item.isSealedGraffiti()) {
            econItem->setStickerID(0, StaticData::paintKits()[item.dataIndex].id);
        } else if (item.isMusic()) {
            econItem->setMusicID(StaticData::paintKits()[item.dataIndex].id);
        } else if (item.isSkin()) {
            if (isKnife(econItem->weaponId))
                econItem->quality = 3;
            econItem->setPaintKit(static_cast<float>(StaticData::paintKits()[item.dataIndex].id));

            const auto& dynamicData = dynamicSkinData[inventory[i].getDynamicDataIndex()];
            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));

            if (dynamicData.statTrak > -1)
            {
                econItem->setStatTrak(dynamicData.statTrak);
                if (!isKnife(econItem->weaponId) && !isGlove(econItem->weaponId))
                    econItem->quality = 9;
            }

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

void updateItem(std::uint64_t itemid)
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto baseTypeCache = localInventory->getItemBaseTypeCache();
    if (!baseTypeCache)
        return;

    const auto view = memory->findOrCreateEconItemViewForItemID(itemid);
    if (!view)
        return;

    const auto soc = memory->getSOCData(view);
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    int itemid_low = soc->itemID - BASE_ITEMID;

    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(soc->weaponId)) {
        if (const auto slotCT = def->getLoadoutSlot(Team::CT); localInventory->getItemInLoadout(Team::CT, slotCT) == view)
            toEquip.emplace_back(Team::CT, slotCT, itemid_low);
        if (const auto slotTT = def->getLoadoutSlot(Team::TT); localInventory->getItemInLoadout(Team::TT, slotTT) == view)
            toEquip.emplace_back(Team::TT, slotTT, itemid_low);
        if (const auto slotNone = def->getLoadoutSlot(Team::None); localInventory->getItemInLoadout(Team::None, slotNone) == view)
            toEquip.emplace_back(Team::None, slotNone, itemid_low);
    }

    removeItemFromInventory(localInventory, baseTypeCache, soc);

    auto& item = inventory[static_cast<std::size_t>(itemid_low)];
    auto itemCopy = item;
    item.markAsDeleted();
    inventory.erase(inventory.begin() + itemid_low);
    inventory.insert(inventory.begin() + itemid_low, std::move(itemCopy));

    sendInventoryUpdatedEvent();
}

void InventoryChanger::updateStatTrak(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    PlayerInfo player_info;
    if (!interfaces->engine->getPlayerInfo(localPlayer->index(), player_info))
        return;

    if (player_info.xuidLow != weapon->xuidLow() || player_info.xuidHigh != weapon->xuidHigh())
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    std::string weapon_name = "weapon_" + std::string(event.getString("weapon"));
    if (weapon_name == "weapon_knife" || weapon_name == "weapon_knife_t")
    {
        const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), 0);
        if (!itemView)
            return;

        const auto soc = memory->getSOCData(itemView);
        if (!soc || !wasItemCreatedByOsiris(soc->itemID))
            return;

        const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(soc->weaponId);
        if (!def)
            return;

        weapon_name = def->getDefinitionName();
        event.setString("weapon", weapon_name.c_str() + 7); //overrideHudIcon
    }

    const auto& definitionIndex = weapon->itemDefinitionIndex2();
    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(definitionIndex);
    if (!def)
        return;

    std::string def_name = def->getDefinitionName();
    if (weapon_name != def_name)
        return;

    const auto loadoutSlot = def->getLoadoutSlot(localPlayer->getTeamNumber());
    const auto itemView = localInventory->getItemInLoadout(localPlayer->getTeamNumber(), loadoutSlot);
    if (!itemView)
        return;

    const auto soc = memory->getSOCData(itemView);
    if (!soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    auto& item = inventory[static_cast<std::size_t>(soc->itemID - BASE_ITEMID)];
    if (!item.isSkin())
        return;

    auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];

    if (dynamicData.statTrak > -1) {
        dynamicData.statTrak++;

        updateItem(soc->itemID);

        const auto attributeList = weapon->econItemView().getAttributeList();
        memory->setOrAddAttributeValueByName(attributeList, "kill eater score type", 0);
        memory->setOrAddAttributeValueByName(attributeList, "kill eater", dynamicData.statTrak);
        weapon->fallbackStatTrak() = dynamicData.statTrak;
        weapon->postDataUpdate(0);
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
    static bool SkinSelectable(const StaticData::GameItem& item, const ImVec2& iconSizeSmall, const ImVec2& iconSizeLarge, ImU32 rarityColor, bool selected = false) noexcept
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

        // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
        ImGuiButtonFlags buttonFlags = 0;

        const bool wasSelected = selected;
        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held, buttonFlags);

        // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
        if (pressed) {
            if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent) {
                SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImRect(bb.Min - window->Pos, bb.Max - window->Pos));
                g.NavDisableHighlight = true;
            }
        }
        if (pressed)
            MarkItemEdited(id);

        // In this branch, Selectable() cannot toggle the selection so this will never trigger.
        if (selected != wasSelected) //-V547
            window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

        if (hovered || selected) {
            const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
            RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
            RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
        }

        if (const auto icon = getItemIconTexture(item.iconPath)) {
            window->DrawList->AddImage(icon, smallIconMin, smallIconMax);
            if (IsMouseHoveringRect(bb.Min, ImVec2{ bb.Min.x + iconSizeSmall.x, bb.Max.y })) {
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

        if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
            CloseCurrentPopup();

        return pressed;
    }
}

enum class pages
{
    list = 0,
    edit,
    add,
    options
};

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
    static int selected_filter = 0;
    static pages pageMode = pages::list;

    if (pageMode == pages::list && ImGui::Button("Add items.."))
        pageMode = pages::add;

    if (pageMode == pages::list) {
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

    static std::vector<addOptions> selectedToAdd;
    static addOptions current_settings;

    if (pageMode == pages::list)
    {
        for (std::size_t i = 0; i < inventory.size(); ++i)
        {
            if (inventory[i].isDeleted())
                continue;

            if (ImGui::BeginChild("##scrollarea_inv", ImVec2{ 0.0f, contentOnly ? 400.0f : 0.0f }))
            {
                ImGui::PushID(i);
                if (ImGui::SkinSelectable(inventory[i].get(), { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(inventory[i].get().rarity), false))
                {
                    current_settings.idx = i;
                    if (inventory[i].get().isSkin())
                    {
                        const auto index = inventory[i].getDynamicDataIndex();
                        current_settings.seed = dynamicSkinData[index].seed;
                        current_settings.wear = dynamicSkinData[index].wear;

                        current_settings.statTrak = dynamicSkinData[index].statTrak > -1;
                        if (current_settings.statTrak)
                            current_settings.statTrak_value = dynamicSkinData[index].statTrak;
                    }
                    else if (inventory[i].get().isGlove())
                    {
                        const auto index = inventory[i].getDynamicDataIndex();
                        current_settings.seed = dynamicGloveData[index].seed;
                        current_settings.wear = dynamicGloveData[index].wear;
                    }

                    pageMode = pages::edit;
                }
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
    }
    else if (pageMode == pages::edit)
    {
        if (ImGui::Button("Back"))
        {
            current_settings = {};
            pageMode = pages::list;
            if (!contentOnly)
                ImGui::End();
            return;
        }

        ImGui::SkinSelectable(inventory[current_settings.idx].get(), { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(inventory[current_settings.idx].get().rarity), false);
        ImGui::Separator();

        if (inventory[current_settings.idx].get().isSkin())
        {
            ImGui::TextUnformatted("Seed");
            ImGui::InputInt("##inv_seed", &current_settings.seed);
            ImGui::TextUnformatted("Wear");
            ImGui::SliderFloat("##inv_wear", &current_settings.wear, FLT_MIN, 1.f, "%.10f");
            ImGui::Checkbox("StatTrak\u2122##inv_stattrak", &current_settings.statTrak);
            if (current_settings.statTrak)
            {
                current_settings.statTrak_value = (std::max)(current_settings.statTrak_value, 0);
                ImGui::InputInt("StatTrak\u2122 Value##inv_stattrak_count", &current_settings.statTrak_value);
            }
            ImGui::Separator();
        }
        else if (inventory[current_settings.idx].get().isGlove())
        {
            ImGui::TextUnformatted("Seed");
            ImGui::InputInt("##inv_seed", &current_settings.seed);
            ImGui::TextUnformatted("Wear");
            ImGui::SliderFloat("##inv_wear", &current_settings.wear, FLT_MIN, 1.f, "%.10f");
            ImGui::Separator();
        }

        if (ImGui::Button("Save"))
        {
            if (inventory[current_settings.idx].get().isSkin())
            {
                const auto index = inventory[current_settings.idx].getDynamicDataIndex();
                dynamicSkinData[index].seed = current_settings.seed;
                dynamicSkinData[index].wear = current_settings.wear;
                dynamicSkinData[index].statTrak = current_settings.statTrak == false ? -1 : current_settings.statTrak_value;
            }
            else if (inventory[current_settings.idx].get().isGlove())
            {
                const auto index = inventory[current_settings.idx].getDynamicDataIndex();
                dynamicGloveData[index].seed = current_settings.seed;
                dynamicGloveData[index].wear = current_settings.wear;
            }

            if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
                if (const auto baseTypeCache = localInventory->getItemBaseTypeCache()) {
                    if (const auto view = memory->getInventoryItemByItemID(localInventory, BASE_ITEMID + current_settings.idx)) {
                        if (const auto econItem = memory->getSOCData(view)) {
                            updateItem(econItem->itemID);
                            InventoryChanger::scheduleHudUpdate();
                        }
                    }
                }
            }

            current_settings = {};
            pageMode = pages::list;
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 20, 20, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(205, 50, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(155, 50, 50, 255));
        if (ImGui::Button("Delete"))
        {
            if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
                if (const auto baseTypeCache = localInventory->getItemBaseTypeCache()) {
                    if (const auto view = memory->getInventoryItemByItemID(localInventory, BASE_ITEMID + current_settings.idx)) {
                        if (const auto econItem = memory->getSOCData(view)) {
                            removeItemFromInventory(localInventory, baseTypeCache, econItem);
                            inventory[current_settings.idx].markAsDeleted();
                            InventoryChanger::scheduleHudUpdate();
                        }
                    }
                }
            }

            sendInventoryUpdatedEvent();
            current_settings = {};
            pageMode = pages::list;
        }
        ImGui::PopStyleColor(4);
    }
    else if (pageMode == pages::add)
    {
        if (ImGui::Button("Back")) {
            pageMode = pages::list;
            selectedToAdd.clear();
            if (!contentOnly)
                ImGui::End();
            return;
        }
        ImGui::SameLine();
        const auto canAdd = !selectedToAdd.empty();
        if (!canAdd) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        if (ImGui::Button(("Add (" + std::to_string(selectedToAdd.size()) + ") items").c_str())) {
            pageMode = pages::list;
            addToInventory(selectedToAdd);
            selectedToAdd.clear();
        }
        if (!canAdd) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120.f);
        ImGui::Combo("##filters", &selected_filter, "All\0Skins\0Stickers\0Knifes\0Gloves\0Agents\0Patches\0Music Kits\0Collectibles\0Graffiti\0Name Tag\0", 6);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##search", "Search..", &filter);

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

                auto& gameItem = gameItems[i];

                if (!filter.empty() && !passesFilter(StaticData::getWeaponNameUpper(gameItem.weaponID), filterWide) && (!gameItem.hasPaintKit() || !passesFilter(StaticData::paintKits()[gameItem.dataIndex].nameUpperCase, filterWide)))
                    continue;

                if (selected_filter == 1 && !gameItem.isSkin())
                    continue;
                if (selected_filter == 2 && !gameItem.isSticker())
                    continue;
                if (selected_filter == 3 && !isKnife(gameItem.weaponID))
                    continue;
                if (selected_filter == 4 && !gameItem.isGlove())
                    continue;
                if (selected_filter == 5 && !gameItem.isAgent())
                    continue;
                if (selected_filter == 6 && !gameItem.isPatch())
                    continue;
                if (selected_filter == 7 && !gameItem.isMusic())
                    continue;
                if (selected_filter == 8 && !gameItem.isCollectible())
                    continue;
                if (selected_filter == 9 && !gameItem.isGraffiti())
                    continue;
                if (selected_filter == 10 && !gameItem.isNameTag())
                    continue;

                ImGui::PushID(i);
                if (ImGui::SkinSelectable(gameItem, { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(gameItem.rarity), false))
                {
                    current_settings.idx = i;
                    pageMode = pages::options;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }
    else if (pageMode == pages::options)
    {
        if (ImGui::Button("Back"))
        {
            current_settings = {};
            pageMode = pages::add;
            if (!contentOnly)
                ImGui::End();
            return;
        }

        ImGui::SkinSelectable(StaticData::gameItems()[current_settings.idx], { 37.0f, 28.0f }, { 200.0f, 150.0f }, rarityColor(StaticData::gameItems()[current_settings.idx].rarity), false);
        ImGui::Separator();

        if (StaticData::gameItems()[current_settings.idx].isSkin())
        {
            ImGui::TextUnformatted("Seed");
            ImGui::InputInt("##inv_seed", &current_settings.seed);
            ImGui::TextUnformatted("Wear");
            ImGui::SliderFloat("##inv_wear", &current_settings.wear, FLT_MIN, 1.f, "%.10f");
            ImGui::Checkbox("StatTrak\u2122##inv_stattrak", &current_settings.statTrak);
            if (current_settings.statTrak)
                current_settings.statTrak_value = 0;
            else
                current_settings.statTrak_value = -1;
            ImGui::Separator();
        }
        else if (StaticData::gameItems()[current_settings.idx].isGlove())
        {
            ImGui::TextUnformatted("Seed");
            ImGui::InputInt("##inv_seed", &current_settings.seed);
            ImGui::TextUnformatted("Wear");
            ImGui::SliderFloat("##inv_wear", &current_settings.wear, FLT_MIN, 1.f, "%.10f");
            ImGui::Separator();
        }
        else if (StaticData::gameItems()[current_settings.idx].isGraffiti() || StaticData::gameItems()[current_settings.idx].isSealedGraffiti())
        {
            if (ImGui::Checkbox("Sealed##sealed_graf", &current_settings.sealed))
            {
                if (current_settings.sealed)
                    current_settings.idx += 286;
                else
                    current_settings.idx -= 286;
            }
            ImGui::Separator();
        }

        if (ImGui::Button("Add"))
        {
            for (int i = 0; i < current_settings.count; i++)
                selectedToAdd.push_back(current_settings);
            current_settings = {};
            pageMode = pages::add;
        }
        ImGui::SameLine();
        current_settings.count = (std::max)(current_settings.count, 1);
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputInt("Count", &current_settings.count);
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

        itemConfig["Apply"] = item.isApply();
        
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

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
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
        case StaticData::Type::Agent: {
            itemConfig["Type"] = "Agent";
            itemConfig["Index"] = gameItem.weaponID;
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
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [stickerID](const auto& gameItem) { return gameItem.isSticker() && StaticData::paintKits()[gameItem.dataIndex].id == stickerID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
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

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);

            auto& dynamicData = dynamicSkinData[inventory.back().getDynamicDataIndex()];

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

            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [paintKit, weaponID](const auto& gameItem) { return gameItem.isGlove() && StaticData::paintKits()[gameItem.dataIndex].id == paintKit && gameItem.weaponID == weaponID; });
            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);

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
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [musicID](const auto& gameItem) { return gameItem.isMusic() && StaticData::paintKits()[gameItem.dataIndex].id == musicID; });

            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
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

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
        } else if (type == "Name Tag") {
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [](const auto& gameItem) { return gameItem.isNameTag(); });
            if (staticData != StaticData::gameItems().end())
                inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
        } else if (type == "Agent") {
            if (!jsonItem.contains("Index") || !jsonItem["Index"].is_number_integer())
                continue;

            const WeaponId index = jsonItem["Index"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [index](const auto& gameItem) { return gameItem.isAgent() && gameItem.weaponID == index; });

            if (staticData == StaticData::gameItems().end())
                continue;

            inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
        } else if (type == "Patch") {
            if (!jsonItem.contains("Patch ID") || !jsonItem["Patch ID"].is_number_integer())
                continue;

            const int patchID = jsonItem["Patch ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [patchID](const auto& gameItem) { return gameItem.isPatch() && StaticData::paintKits()[gameItem.dataIndex].id == patchID; });
            if (staticData != StaticData::gameItems().end())
                inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
        } else if (type == "Graffiti") {
            if (!jsonItem.contains("Graffiti ID") || !jsonItem["Graffiti ID"].is_number_integer())
                continue;

            const int graffitiID = jsonItem["Graffiti ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [graffitiID](const auto& gameItem) { return gameItem.isGraffiti() && StaticData::paintKits()[gameItem.dataIndex].id == graffitiID; });
            if (staticData != StaticData::gameItems().end())
                inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
        } else if (type == "Sealed Graffiti") {
            if (!jsonItem.contains("Graffiti ID") || !jsonItem["Graffiti ID"].is_number_integer())
                continue;

            const int graffitiID = jsonItem["Graffiti ID"];
            const auto staticData = std::ranges::find_if(StaticData::gameItems(), [graffitiID](const auto& gameItem) { return gameItem.isSealedGraffiti() && StaticData::paintKits()[gameItem.dataIndex].id == graffitiID; });
            if (staticData != StaticData::gameItems().end())
                inventory.emplace_back(std::ranges::distance(StaticData::gameItems().begin(), staticData), (jsonItem.contains("Apply") && jsonItem["Apply"] == true) ? true : false);
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
    dynamicGloveData.clear();

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

    if (const auto soc = memory->getSOCData(itemView); !soc || !wasItemCreatedByOsiris(soc->itemID))
        return;

    sequence = remapKnifeAnim(viewModelWeapon->itemDefinitionIndex2(), sequence);
}

StaticData::GameItem::GameItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath) noexcept : type{ type }, rarity{ static_cast<std::uint8_t>(rarity) }, weaponID{ weaponID }, dataIndex{ dataIndex }, iconPath{ std::move(iconPath) } {}

StaticData::PaintKit::PaintKit(int id, std::wstring&& name) noexcept : id{ id }, nameUpperCase{ std::move(name) }
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}

static bool needUpdate = false;

void InventoryChanger::swapTeam(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (event.getInt("userid") != localPlayer->getUserId())
        return;

    needUpdate = true;
}

void InventoryChanger::playerSpawn(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (event.getInt("userid") != localPlayer->getUserId())
        return;

    if (needUpdate)
    {
        InventoryChanger::scheduleHudUpdate();
        needUpdate = false;
    }
}

void InventoryChanger::setItemBackPackPosition(void* item_view) noexcept
{
    const auto econItem = memory->getSOCData(item_view);

    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();

    if (!localInventory)
        return;

    static const auto baseInvID = localInventory->getHighestIDs().second;

    if (econItem->inventory == 0)
    {
        econItem->inventory = baseInvID + (econItem->itemID - BASE_ITEMID) + 1;
        inventory[econItem->itemID - BASE_ITEMID].markAsApply();

        sendInventoryUpdatedEvent();
    }
}
