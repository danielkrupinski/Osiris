#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
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

/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

struct sticker_setting
{
    void update()
    {
        // kit = SkinChanger::getStickerKits()[kit_vector_index].id;
    }

    void onLoad()
    {
        /*
        const auto& kits = SkinChanger::getStickerKits();
        const auto it = std::find_if(kits.begin(), kits.end(), [this](const auto& k) { return k.id == kit; });
        kit_vector_index = it != SkinChanger::getStickerKits().end() ? std::distance(kits.begin(), it) : 0;
        kit = SkinChanger::getStickerKits()[kit_vector_index].id;
        */
    }

    auto operator==(const sticker_setting& o) const
    {
        return kit == o.kit
            && kit_vector_index == o.kit_vector_index
            && wear == o.wear
            && scale == o.scale
            && rotation == o.rotation;
    }

    int kit = 0;
    int kit_vector_index = 0;
    float wear = (std::numeric_limits<float>::min)();
    float scale = 1.f;
    float rotation = 0.f;
};

struct item_setting {
    void update()
    {
        itemId = SkinChanger::weapon_names[itemIdIndex].definition_index;
        quality = SkinChanger::getQualities()[entity_quality_vector_index].index;

        if (itemId == WeaponId::GloveT) {
            // paintKit = SkinChanger::getGloveKits()[paint_kit_vector_index].id;
            definition_override_index = (int)SkinChanger::getGloveTypes()[definition_override_vector_index].id;
        } else {
            // paintKit = SkinChanger::getSkinKits()[paint_kit_vector_index].id;
            definition_override_index = (int)SkinChanger::getKnifeTypes()[definition_override_vector_index].id;
        }

        for (auto& sticker : stickers)
            sticker.update();
    }

    void onLoad()
    {
        {
            const auto it = std::find_if(std::begin(SkinChanger::weapon_names), std::end(SkinChanger::weapon_names), [this](const auto& k) { return k.definition_index == itemId; });
            itemIdIndex = it != std::end(SkinChanger::weapon_names) ? std::distance(std::begin(SkinChanger::weapon_names), it) : 0;
        }

        {
            const auto& qualities = SkinChanger::getQualities();
            const auto it = std::find_if(qualities.begin(), qualities.end(), [this](const auto& k) { return k.index == quality; });
            entity_quality_vector_index = it != qualities.end() ? std::distance(qualities.begin(), it) : 0;
        }

        if (itemId == WeaponId::GloveT) {
            {
               // const auto it = std::find_if(SkinChanger::getGloveKits().begin(), SkinChanger::getGloveKits().end(), [this](const auto& k) { return k.id == paintKit; });
               // paint_kit_vector_index = it != SkinChanger::getGloveKits().end() ? std::distance(SkinChanger::getGloveKits().begin(), it) : 0;
            }

            {
                const auto it = std::find_if(SkinChanger::getGloveTypes().begin(), SkinChanger::getGloveTypes().end(), [this](const auto& k) { return (int)k.id == definition_override_index; });
                definition_override_vector_index = it != SkinChanger::getGloveTypes().end() ? std::distance(SkinChanger::getGloveTypes().begin(), it) : 0;
            }
        } else {
            {
                // const auto it = std::find_if(SkinChanger::getSkinKits().begin(), SkinChanger::getSkinKits().end(), [this](const auto& k) { return k.id == paintKit; });
                // paint_kit_vector_index = it != SkinChanger::getSkinKits().end() ? std::distance(SkinChanger::getSkinKits().begin(), it) : 0;
            }

            {
                const auto it = std::find_if(SkinChanger::getKnifeTypes().begin(), SkinChanger::getKnifeTypes().end(), [this](const auto& k) { return (int)k.id == definition_override_index; });
                definition_override_vector_index = it != SkinChanger::getKnifeTypes().end() ? std::distance(SkinChanger::getKnifeTypes().begin(), it) : 0;
            }
        }

        for (auto& sticker : stickers)
            sticker.onLoad();
    }

    bool enabled = false;
    int itemIdIndex = 0;
    WeaponId itemId{};
    int entity_quality_vector_index = 0;
    int quality = 0;
    int paint_kit_vector_index = 0;
    int paintKit = 0;
    int definition_override_vector_index = 0;
    int definition_override_index = 0;
    int seed = 0;
    int stat_trak = -1;
    float wear = (std::numeric_limits<float>::min)();
    char custom_name[32] = "";
    std::array<sticker_setting, 5> stickers;
};

static std::array<item_setting, 36> skinChangerConfig;

static constexpr auto is_knife(WeaponId id)
{
    return (id >= WeaponId::Bayonet && id < WeaponId::GloveStuddedBloodhound) || id == WeaponId::KnifeT || id == WeaponId::Knife;
}

static item_setting* get_by_definition_index(WeaponId weaponId)
{
    const auto it = std::ranges::find(skinChangerConfig, weaponId, &item_setting::itemId);
    return (it == skinChangerConfig.end() || !it->enabled) ? nullptr : &*it;
}

static std::vector<SkinChanger::GameItem> gameItems;
static std::vector<SkinChanger::StickerData> stickerData;
static std::vector<SkinChanger::GloveData> gloveData;
static std::vector<SkinChanger::SkinData> skinData;

struct StickerConfig {
    int stickerID = 0;
    float wear = 0.0f;
};

struct DynamicSkinData {
    std::array<StickerConfig, 5> stickers;
};

static std::vector<DynamicSkinData> dynamicSkinData;

struct InventoryItem {
private:
    std::size_t itemIndex;
    std::size_t dynamicDataIndex = static_cast<std::size_t>(-1);
public:
    InventoryItem(std::size_t itemIndex) : itemIndex{ itemIndex } {}

    void markAsDeleted() noexcept { itemIndex = static_cast<std::size_t>(-1); }
    bool isDeleted() const noexcept { return itemIndex == static_cast<std::size_t>(-1); }
    
    bool isSticker() const noexcept { return !isDeleted() && gameItems[itemIndex].type == SkinChanger::GameItem::Type::Sticker; }
    bool isSkin() const noexcept { return !isDeleted() && gameItems[itemIndex].type == SkinChanger::GameItem::Type::Skin; }
    bool isGlove() const noexcept { return !isDeleted() && gameItems[itemIndex].type == SkinChanger::GameItem::Type::Glove; }

    bool hasDynamicData() const noexcept { return dynamicDataIndex != static_cast<std::size_t>(-1); }
    std::size_t getDynamicDataIndex() const noexcept { assert(hasDynamicData()); return dynamicDataIndex; }
    void setDynamicDataIndex(std::size_t newDynamicDataIndex) noexcept { dynamicDataIndex = newDynamicDataIndex; }

    SkinChanger::GameItem& get() const noexcept
    {
        assert(!isDeleted());
        return gameItems[itemIndex];
    }
};

static std::vector<InventoryItem> inventory;

static void addToInventory(const std::unordered_set<std::size_t>& indicesToAdd) noexcept
{
    for (const auto idx : indicesToAdd) {
        inventory.emplace_back(idx);
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
        if (std::string_view name{ stickerKit->name.data() }; name.starts_with("spray") || name.starts_with("patch") || name.ends_with("graffiti"))
            continue;
        std::wstring name = interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() + 1 : "StickerKit_dhw2014_teamdignitas_gold");

        stickerData.emplace_back(stickerKit->id);
        gameItems.emplace_back(SkinChanger::GameItem::Type::Sticker, stickerKit->rarity, stickerData.size() - 1, std::move(name), stickerKit->inventoryImage.data());
    }

    std::ranges::sort(gameItems, {}, &SkinChanger::GameItem::nameUpperCase);
    gameItems.shrink_to_fit();
}

void apply_sticker_changer(Entity* item) noexcept
{
    if (auto config = get_by_definition_index(item->itemDefinitionIndex2())) {
        constexpr auto m_Item = fnv::hash("CBaseAttributableItem->m_Item");
        const auto attributeList = std::uintptr_t(item) + netvars->operator[](m_Item) + /* m_AttributeList = */ WIN32_LINUX(0x244, 0x2F8);

        for (std::size_t i = 0; i < config->stickers.size(); ++i) {
            const auto& sticker = config->stickers[i];
            const auto attributeString = "sticker slot " + std::to_string(i) + ' ';

            memory->setOrAddAttributeValueByName(attributeList, (attributeString + "id").c_str(), sticker.kit);
            memory->setOrAddAttributeValueByName(attributeList, (attributeString + "wear").c_str(), sticker.wear);
            memory->setOrAddAttributeValueByName(attributeList, (attributeString + "scale").c_str(), sticker.scale);
            memory->setOrAddAttributeValueByName(attributeList, (attributeString + "rotation").c_str(), sticker.rotation);
        }
    }
}

static void apply_config_on_attributable_item(Entity* item, const item_setting& config,
    const unsigned xuid_low) noexcept
{
    // Force fallback values to be used.
    item->itemIDHigh() = -1;

    // Set the owner of the weapon to our lower XUID. (fixes StatTrak)
    item->accountID() = xuid_low;
    item->entityQuality() = config.quality;

    if (config.stat_trak > -1) {
        item->fallbackStatTrak() = config.stat_trak;
        item->entityQuality() = 9;
    }

    if (is_knife(item->itemDefinitionIndex2()))
        item->entityQuality() = 3; // make a star appear on knife

    if (config.custom_name[0])
        std::strncpy(item->customName(), config.custom_name, 32);

    if (config.paintKit)
        item->fallbackPaintKit() = config.paintKit;

    if (config.seed)
        item->fallbackSeed() = config.seed;

    item->fallbackWear() = config.wear;

    if (auto& definition_index = item->itemDefinitionIndex(); config.definition_override_index && config.definition_override_index != definition_index) {
        definition_index = config.definition_override_index;
        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(WeaponId{ definition_index })) {
            item->setModelIndex(interfaces->modelInfo->getModelIndex(config.itemId == WeaponId::GloveT ? def->getWorldDisplayModel() : def->getPlayerDisplayModel()));
            item->preDataUpdate(0);
        }
    }
    apply_sticker_changer(item);
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

static void post_data_update_start(int localHandle) noexcept
{
    const auto local = interfaces->entityList->getEntityFromHandle(localHandle);
    if (!local)
        return;

    const auto local_index = local->index();

    if (!local->isAlive())
        return;

    PlayerInfo player_info;
    if (!interfaces->engine->getPlayerInfo(local_index, player_info))
        return;

    // Handle glove config
    {
        const auto wearables = local->wearables();

        const auto glove_config = get_by_definition_index(WeaponId::GloveT);

        static int glove_handle;

        auto glove = interfaces->entityList->getEntityFromHandle(wearables[0]);

        if (!glove) // There is no glove
        {
            // Try to get our last created glove
            const auto our_glove = interfaces->entityList->getEntityFromHandle(glove_handle);

            if (our_glove) // Our glove still exists
            {
                wearables[0] = glove_handle;
                glove = our_glove;
            }
        }

        if (glove_config && glove_config->definition_override_index)
        {
            // We don't have a glove, but we should
            if (!glove)
            {
                auto entry = interfaces->entityList->getHighestEntityIndex() + 1;
#define HIJACK_ENTITY 1
#if HIJACK_ENTITY == 1
                for (int i = 65; i <= interfaces->entityList->getHighestEntityIndex(); i++) {
                    auto entity = interfaces->entityList->getEntity(i);

                    if (entity && entity->getClientClass()->classId == ClassId{ 70 }) {
                        entry = i;
                        break;
                    }
                }
#endif
                const auto serial = rand() % 0x1000;

                glove = make_glove(entry, serial);
                if (glove) {
                    glove->initialized() = true;

                    wearables[0] = entry | serial << 16;

                    // Let's store it in case we somehow lose it.
                    glove_handle = wearables[0];
                }
            }

            if (glove) {
                memory->equipWearable(glove, local);
                local->body() = 1;

                apply_config_on_attributable_item(glove, *glove_config, player_info.xuidLow);
            }
        }
    }

    // Handle weapon configs
    {
        auto& weapons = local->weapons();

        for (auto weapon_handle : weapons) {
            if (weapon_handle == -1)
                break;

            auto weapon = interfaces->entityList->getEntityFromHandle(weapon_handle);

            if (!weapon)
                continue;

            auto& weaponId = weapon->itemDefinitionIndex2();

            // All knives are terrorist knives.
            if (const auto active_conf = get_by_definition_index(is_knife(weaponId) ? WeaponId::Knife : weaponId))
                apply_config_on_attributable_item(weapon, *active_conf, player_info.xuidLow);
        }
    }

    const auto view_model = interfaces->entityList->getEntityFromHandle(local->viewModel());

    if (!view_model)
        return;

    const auto view_model_weapon = interfaces->entityList->getEntityFromHandle(view_model->weapon());

    if (!view_model_weapon)
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(view_model_weapon->itemDefinitionIndex2());
    if (!def)
        return;

    view_model->modelIndex() = interfaces->modelInfo->getModelIndex(def->getPlayerDisplayModel());

    const auto world_model = interfaces->entityList->getEntityFromHandle(view_model_weapon->weaponWorldModel());

    if (!world_model)
        return;

    world_model->modelIndex() = interfaces->modelInfo->getModelIndex(def->getWorldDisplayModel());
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
    useToolTime = memory->globalVars->realtime + 1.1f; // TODO: randomize the delay
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
        post_data_update_start(localPlayerHandle);
        if (hudUpdateRequired && localPlayer && !localPlayer->isDormant())
            updateHud();
    }

    if (stage != FrameStage::RENDER_START)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    const auto soc = localInventory->getSOC();
    if (!soc)
        return;

    const auto baseTypeCache = soc->findBaseTypeCache(1);
    if (!baseTypeCache)
        return;

    static const auto [baseItemID, baseInvID] = localInventory->getHighestIDs();

    if (useToolTime <= memory->globalVars->realtime) {
        if (toolToUse > baseItemID && itemToApplyTool > baseItemID) {
            auto& tool = inventory[static_cast<std::size_t>(toolToUse - baseItemID - 1)];
            const auto& toolItem = tool.get();
            auto& dest = inventory[static_cast<std::size_t>(itemToApplyTool - baseItemID - 1)];
            const auto& destItem = dest.get();

            if (toolItem.type == SkinChanger::GameItem::Type::Sticker) {
                const auto& sticker = stickerData[toolItem.dataIndex];
                if (const auto view = memory->findOrCreateEconItemViewForItemID(itemToApplyTool)) {
                    memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), ("sticker slot " + std::to_string(slotToApplySticker) + " id").c_str(), sticker.stickerID);
                    memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), ("sticker slot " + std::to_string(slotToApplySticker) + " wear").c_str(), 0.10f);
                    memory->clearInventoryImageRGBA(view);

                    if (!dest.hasDynamicData()) {
                        dynamicSkinData.push_back({});
                        dest.setDynamicDataIndex(dynamicSkinData.size() - 1);
                    }
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
        if (inventory[i].isDeleted() || memory->getInventoryItemByItemID(localInventory, baseItemID + i + 1))
            continue;

        const auto& item = inventory[i].get();
       
        const auto econItem = memory->createEconItemSharedObject();
        econItem->itemID = baseItemID + i + 1;
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
        }

        baseTypeCache->addObject(econItem);
        memory->addEconItem(localInventory, econItem, false, false, false);

        if (const auto view = memory->findOrCreateEconItemViewForItemID(econItem->itemID)) {
            if (item.type == SkinChanger::GameItem::Type::Sticker) {
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "sticker slot 0 id", stickerData[item.dataIndex].stickerID);
            } else {
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "set item texture prefab", static_cast<float>(item.type == SkinChanger::GameItem::Type::Skin ? skinData[item.dataIndex].paintKit : gloveData[item.dataIndex].paintKit));
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "set item texture wear", 0.01f);
                memory->setOrAddAttributeValueByName(std::uintptr_t(view) + WIN32_LINUX(0x244, 0x2F8), "set item texture seed", static_cast<float>(1));
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

    if (const auto active_conf = get_by_definition_index(WeaponId::Knife)) {
        if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(WeaponId(active_conf->definition_override_index))) {
            if (const auto defName = def->getDefinitionName(); defName && std::string_view{ defName }.starts_with("weapon_"))
                event.setString("weapon", defName + 7);
        }
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

    if (const auto conf = get_by_definition_index(is_knife(weapon->itemDefinitionIndex2()) ? WeaponId::Knife : weapon->itemDefinitionIndex2()); conf && conf->stat_trak > -1) {
        weapon->fallbackStatTrak() = ++conf->stat_trak;
        weapon->postDataUpdate(0);
    }
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

#ifdef SKINCHANGER_REWORK
    static std::string filter;

    static bool isInAddMode = false;

    if (!isInAddMode && ImGui::Button("Add items.."))
        isInAddMode = true;

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
        ImGui::InputTextWithHint("##search", "Search weapon skins, stickers, knives, gloves..", &filter);

        if (ImGui::BeginChild("##scrollarea")) {
            const auto& gameItems = getGameItems();
            for (std::size_t i = 0; i < gameItems.size(); ++i) {
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
#else

    static auto itemIndex = 0;

    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
        *out_text = SkinChanger::weapon_names[idx].name;
        return true;
        }, nullptr, SkinChanger::weapon_names.size(), 5);
    ImGui::PopItemWidth();

    auto& selected_entry = skinChangerConfig[itemIndex];
    selected_entry.itemIdIndex = itemIndex;

    constexpr auto rarityColor = [](int rarity) {
        constexpr auto rarityColors = std::to_array<ImU32>({
            IM_COL32(  0,   0,   0,   0),
            IM_COL32(176, 195, 217, 255),
            IM_COL32( 94, 152, 217, 255),
            IM_COL32( 75, 105, 255, 255),
            IM_COL32(136,  71, 255, 255),
            IM_COL32(211,  44, 230, 255),
            IM_COL32(235,  75,  75, 255),
            IM_COL32(228, 174,  57, 255)
        });
        return rarityColors[static_cast<std::size_t>(rarity) < rarityColors.size() ? rarity : 0];
    };

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

    {
        ImGui::SameLine();
        ImGui::Checkbox("Enabled", &selected_entry.enabled);
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);
        ImGui::InputInt("Seed", &selected_entry.seed);
        ImGui::InputInt("StatTrak\u2122", &selected_entry.stat_trak);
        selected_entry.stat_trak = (std::max)(selected_entry.stat_trak, -1);
        ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", ImGuiSliderFlags_Logarithmic);

        const auto& kits = itemIndex == 1 ? SkinChanger::getGloveKits() : SkinChanger::getSkinKits();

        ImGui::SetNextItemWidth(270.0f);
        if (ImGui::BeginCombo("Paint Kit", kits[selected_entry.paint_kit_vector_index].name.c_str())) {
            ImGui::PushID("Paint Kit");
            ImGui::PushID("Search");
            ImGui::SetNextItemWidth(-1.0f);
            static std::array<std::string, SkinChanger::weapon_names.size()> filters;
            auto& filter = filters[itemIndex];
            ImGui::InputTextWithHint("", "Search", &filter);
            if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
                ImGui::SetKeyboardFocusHere(-1);
            ImGui::PopID();

            const std::wstring filterWide = Helpers::toUpper(Helpers::toWideString(filter));
            if (ImGui::BeginChild("##scrollarea", { 0, 6 * ImGui::GetTextLineHeightWithSpacing() })) {
                for (std::size_t i = 0; i < kits.size(); ++i) {
                    if (filter.empty() || passesFilter(kits[i].nameUpperCase, filterWide)) {
                        ImGui::PushID(i);
                        const auto selected = i == selected_entry.paint_kit_vector_index;
                        if (ImGui::SkinSelectable(kits[i].name.c_str(), kits[i].iconPath, { 35.0f, 26.25f }, { 200.0f, 150.0f }, rarityColor(kits[i].rarity), selected)) {
                            selected_entry.paint_kit_vector_index = i;
                            ImGui::CloseCurrentPopup();
                        }
                        if (selected && ImGui::IsWindowAppearing())
                            ImGui::SetScrollHereY();
                        ImGui::PopID();
                    }
                }
            }
            ImGui::EndChild();
            ImGui::PopID();
            ImGui::EndCombo();
        }

        ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = SkinChanger::getQualities()[idx].name.c_str(); // safe within this lamba
            return true;
            }, nullptr, SkinChanger::getQualities().size(), 5);

        if (itemIndex == 0) {
            ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = SkinChanger::getKnifeTypes()[idx].name.c_str();
                return true;
                }, nullptr, SkinChanger::getKnifeTypes().size(), 5);
        } else if (itemIndex == 1) {
            ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = SkinChanger::getGloveTypes()[idx].name.c_str();
                return true;
                }, nullptr, SkinChanger::getGloveTypes().size(), 5);
        } else {
            static auto unused_value = 0;
            selected_entry.definition_override_vector_index = 0;
            ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
        }

        ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
    }

    ImGui::NextColumn();

    {
        ImGui::PushID("sticker");

        static std::size_t selectedStickerSlot = 0;

        ImGui::PushItemWidth(-1);
        ImVec2 size;
        size.x = 0.0f;
        size.y = ImGui::GetTextLineHeightWithSpacing() * 5.25f + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::BeginListBox("", size)) {
            for (int i = 0; i < 5; ++i) {
                ImGui::PushID(i);

                const auto kit_vector_index = skinChangerConfig[itemIndex].stickers[i].kit_vector_index;
                const std::string text = '#' + std::to_string(i + 1) + "  " + SkinChanger::getStickerKits()[kit_vector_index].name;

                if (ImGui::Selectable(text.c_str(), i == selectedStickerSlot))
                    selectedStickerSlot = i;

                ImGui::PopID();
            }
            ImGui::EndListBox();
        }

        ImGui::PopItemWidth();

        auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

        const auto& kits = SkinChanger::getStickerKits();
        ImGui::SetNextItemWidth(270.0f);
        if (ImGui::BeginCombo("Sticker", kits[selected_sticker.kit_vector_index].name.c_str())) {
            ImGui::PushID("Sticker");
            ImGui::PushID("Search");
            ImGui::SetNextItemWidth(-1.0f);
            static std::array<std::string, SkinChanger::weapon_names.size()> filters;
            auto& filter = filters[itemIndex];
            ImGui::InputTextWithHint("", "Search", &filter);
            if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
                ImGui::SetKeyboardFocusHere(-1);
            ImGui::PopID();

            const std::wstring filterWide = Helpers::toUpper(Helpers::toWideString(filter));
            if (ImGui::BeginChild("##scrollarea", { 0, 6 * ImGui::GetTextLineHeightWithSpacing() })) {
                for (std::size_t i = 0; i < kits.size(); ++i) {
                    if (filter.empty() || passesFilter(kits[i].nameUpperCase, filterWide)) {
                        ImGui::PushID(i);
                        const auto selected = i == selected_sticker.kit_vector_index;
                        if (ImGui::SkinSelectable(kits[i].name.c_str(), kits[i].iconPath, { 35.0f, 26.25f }, { 200.0f, 150.0f }, rarityColor(kits[i].rarity), selected)) {
                            selected_sticker.kit_vector_index = i;
                            ImGui::CloseCurrentPopup();
                        }

                        if (selected && ImGui::IsWindowAppearing())
                            ImGui::SetScrollHereY();
                        ImGui::PopID();
                    }
                }
            }
            ImGui::EndChild();
            ImGui::PopID();
            ImGui::EndCombo();
        }

        ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.0f);
        ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.0f, 360.0f);

        ImGui::PopID();
    }
    selected_entry.update();

    ImGui::Columns(1);

    ImGui::Separator();

    if (ImGui::Button("Update", { 130.0f, 30.0f }))
        SkinChanger::scheduleHudUpdate();

    ImGui::TextUnformatted("nSkinz by namazso");
#endif
    if (!contentOnly)
        ImGui::End();
}

static void to_json(json& j, const sticker_setting& o)
{
    const sticker_setting dummy;

    WRITE("Kit", kit);
    WRITE("Wear", wear);
    WRITE("Scale", scale);
    WRITE("Rotation", rotation);
}

static void to_json(json& j, const item_setting& o)
{
    const item_setting dummy;

    WRITE("Enabled", enabled);
    WRITE("Definition index", itemId);
    WRITE("Quality", quality);
    WRITE("Paint Kit", paintKit);
    WRITE("Definition override", definition_override_index);
    WRITE("Seed", seed);
    WRITE("StatTrak", stat_trak);
    WRITE("Wear", wear);
    if (o.custom_name[0])
        j["Custom name"] = o.custom_name;
    WRITE("Stickers", stickers);
}

json SkinChanger::toJson() noexcept
{
    return skinChangerConfig;
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

            if (item.hasDynamicData()) {
                const auto& dynamicData = dynamicSkinData[item.getDynamicDataIndex()];
                for (const auto& sticker : dynamicData.stickers) {
                    if (sticker.stickerID == 0)
                        continue;

                    json stickerConfig;
                    stickerConfig["Sticker ID"] = sticker.stickerID;
                    stickerConfig["Wear"] = sticker.wear;
                    itemConfig["Stickers"].push_back(stickerConfig);
                }
            }
            break;
        }
        }

        j.push_back(itemConfig);
    }
    return j;
}

static void from_json(const json& j, sticker_setting& s)
{
    read(j, "Kit", s.kit);
    read(j, "Wear", s.wear);
    read(j, "Scale", s.scale);
    read(j, "Rotation", s.rotation);

    s.onLoad();
}

static void from_json(const json& j, item_setting& i)
{
    read(j, "Enabled", i.enabled);
    read(j, "Definition index", i.itemId);
    read(j, "Quality", i.quality);
    read(j, "Paint Kit", i.paintKit);
    read(j, "Definition override", i.definition_override_index);
    read(j, "Seed", i.seed);
    read(j, "StatTrak", i.stat_trak);
    read(j, "Wear", i.wear);
    read(j, "Custom name", i.custom_name, sizeof(i.custom_name));
    read(j, "Stickers", i.stickers);

    i.onLoad();
}

void SkinChanger::fromJson(const json& j) noexcept
{
    if (j.type() == value_t::array && j.size() == skinChangerConfig.size()) {
        for (std::size_t i = 0; i < j.size(); ++i) {
            if (!j[i].empty())
                j[i].get_to(skinChangerConfig[i]);
        }
    }
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

                if (!inventory.back().hasDynamicData()) {
                    dynamicSkinData.push_back({});
                    inventory.back().setDynamicDataIndex(dynamicSkinData.size() - 1);
                }

                dynamicSkinData[inventory.back().getDynamicDataIndex()].stickers[k].stickerID = stickerID;
                if (sticker.contains("Wear") && sticker["Wear"].is_number_float())
                    dynamicSkinData[inventory.back().getDynamicDataIndex()].stickers[k].wear = sticker["Wear"];
            }
        } else if (type == "Glove") {

        }
    }
}

void SkinChanger::resetConfig() noexcept
{
    skinChangerConfig = {};
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

const std::vector<SkinChanger::Quality>& SkinChanger::getQualities() noexcept
{
    static std::vector<Quality> qualities;
    if (qualities.empty()) {
        for (const auto& node : memory->itemSystem()->getItemSchema()->qualities) {
            const auto quality = node.value;
            if (const auto localizedName = interfaces->localize->findAsUTF8(quality.name); localizedName != quality.name)
                qualities.emplace_back(quality.value, localizedName);
        }

        if (qualities.empty()) // fallback
            qualities.emplace_back(0, "Default");
    }

    return qualities;
}

const std::vector<SkinChanger::Item>& SkinChanger::getGloveTypes() noexcept
{
    static std::vector<SkinChanger::Item> gloveTypes;
    if (gloveTypes.empty()) {
        gloveTypes.emplace_back(WeaponId{}, "Default");

        for (const auto& node : memory->itemSystem()->getItemSchema()->itemsSorted) {
            const auto item = node.value;
            if (std::strcmp(item->getItemTypeName(), "#Type_Hands") == 0 && item->isPaintable())
                gloveTypes.emplace_back(item->getWeaponId(), interfaces->localize->findAsUTF8(item->getItemBaseName()));
        }
    }

    return gloveTypes;
}

const std::vector<SkinChanger::Item>& SkinChanger::getKnifeTypes() noexcept
{
    static std::vector<SkinChanger::Item> knifeTypes;
    if (knifeTypes.empty()) {
        knifeTypes.emplace_back(WeaponId{}, "Default");

        for (const auto& node : memory->itemSystem()->getItemSchema()->itemsSorted) {
            const auto item = node.value;
            if (std::strcmp(item->getItemTypeName(), "#CSGO_Type_Knife") == 0 && item->getRarity() == 6)
                knifeTypes.emplace_back(item->getWeaponId(), interfaces->localize->findAsUTF8(item->getItemBaseName()));
        }
    }

    return knifeTypes;
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
        if (const auto handle = interfaces->baseFileSystem->open(("resource/flash/" + iconpath + "_large.png").c_str(), "r", "GAME")) {
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
        } else {
            assert(false);
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

SkinChanger::PaintKit::PaintKit(int id, std::wstring&& name, std::string&& iconPath, WeaponId weaponId, ItemType type, int rarity) noexcept : id{ id }, nameUpperCase{ std::move(name) }, iconPath{ std::move(iconPath) }, weaponId{ weaponId }, type{ type }, rarity{ rarity }
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}

SkinChanger::PaintKit::PaintKit(int id, std::wstring&& name, WeaponId weaponId, ItemType type, int rarity) noexcept : id{ id }, nameUpperCase{ std::move(name) }, weaponId{ weaponId }, type{ type }, rarity{ rarity }
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
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
    if (!is_knife(viewModelWeapon->itemDefinitionIndex2()))
        return;

    const auto active_conf = get_by_definition_index(WeaponId::Knife);
    if (!active_conf || !active_conf->definition_override_index)
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(WeaponId(active_conf->definition_override_index));
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
