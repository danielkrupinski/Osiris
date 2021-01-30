#include <algorithm>
#include <cstring>
#include <cwctype>
#include <fstream>
#include <unordered_set>

#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../imgui/imgui.h"

#ifdef _WIN32
#include "../imgui/imgui_impl_dx9.h"
#else
#include "../imgui/imgui_impl_opengl3.h"
#endif

#include "../Interfaces.h"

#include "SkinChanger.h"
#include "../Config.h"

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
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/ModelInfo.h"
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

static constexpr auto is_knife(WeaponId id)
{
    return (id >= WeaponId::Bayonet && id < WeaponId::GloveStuddedBloodhound) || id == WeaponId::KnifeT || id == WeaponId::Knife;
}

item_setting* get_by_definition_index(WeaponId weaponId)
{
    const auto it = std::find_if(config->skinChanger.begin(), config->skinChanger.end(), [weaponId](const item_setting& e) { return e.enabled && e.itemId == weaponId; });
    return it == config->skinChanger.end() ? nullptr : &*it;
}

static std::vector<SkinChanger::PaintKit> skinKits{ { 0, "-" } };
static std::vector<SkinChanger::PaintKit> gloveKits;

static void initializeKits() noexcept
{
    static bool initalized = false;
    if (initalized)
        return;
    initalized = true;

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

    std::sort(kitsWeapons.begin(), kitsWeapons.end(), [](const auto& a, const auto& b) { return a.paintKit < b.paintKit; });
 
    skinKits.reserve(itemSchema->paintKits.lastAlloc);
    gloveKits.reserve(itemSchema->paintKits.lastAlloc);
    for (const auto& node : itemSchema->paintKits) {
        const auto paintKit = node.value;

        if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
            continue;

        if (paintKit->id >= 10000) {
            std::wstring name;
            std::string iconPath;

            if (const auto it = std::lower_bound(kitsWeapons.begin(), kitsWeapons.end(), paintKit->id, [](const auto& p, auto id) { return p.paintKit < id; }); it != kitsWeapons.end() && it->paintKit == paintKit->id) {
                if (const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId)) {
                    name = interfaces->localize->findSafe(itemDef->getItemBaseName());
                    name += L" | ";
                }
                iconPath = it->iconPath;
            }

            name += interfaces->localize->findSafe(paintKit->itemName.data() + 1);
            gloveKits.emplace_back(paintKit->id, std::move(name), std::move(iconPath), paintKit->rarity);
        } else {
            for (auto it = std::lower_bound(kitsWeapons.begin(), kitsWeapons.end(), paintKit->id, [](const auto& p, auto id) { return p.paintKit < id; }); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {

                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                std::wstring name = interfaces->localize->findSafe(itemDef->getItemBaseName());
                name += L" | ";
                name += interfaces->localize->findSafe(paintKit->itemName.data() + 1);
                skinKits.emplace_back(paintKit->id, std::move(name), it->iconPath, std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6));
            }
        }
    }

    std::sort(skinKits.begin() + 1, skinKits.end());
    skinKits.shrink_to_fit();
    std::sort(gloveKits.begin(), gloveKits.end());
    gloveKits.shrink_to_fit();
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

const std::vector<SkinChanger::PaintKit>& SkinChanger::getSkinKits() noexcept
{
    initializeKits();
    return skinKits;
}

const std::vector<SkinChanger::PaintKit>& SkinChanger::getGloveKits() noexcept
{
    initializeKits();
    return gloveKits;
}

const std::vector<SkinChanger::PaintKit>& SkinChanger::getStickerKits() noexcept
{
    static std::vector<SkinChanger::PaintKit> stickerKits;
    if (stickerKits.empty()) {
        const auto& stickerMap = memory->itemSystem()->getItemSchema()->stickerKits;

        stickerKits.reserve(stickerMap.numElements + 1);
        stickerKits.emplace_back(0, "None");

        for (const auto& node : stickerMap) {
            const auto stickerKit = node.value;
            if (std::string_view name{ stickerKit->name.data() }; name.starts_with("spray") || name.starts_with("patch") || name.ends_with("graffiti"))
                continue;
            std::wstring name = interfaces->localize->findSafe(stickerKit->id != 242 ? stickerKit->itemName.data() + 1 : "StickerKit_dhw2014_teamdignitas_gold");
            stickerKits.emplace_back(stickerKit->id, std::move(name), stickerKit->inventoryImage.data(), stickerKit->rarity);
        }

        std::sort(stickerKits.begin() + 1, stickerKits.end());
        stickerKits.shrink_to_fit();
    }
    return stickerKits;
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

class Texture {
    ImTextureID texture = nullptr;
public:
    Texture() = default;
    ~Texture() { clear(); }
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept : texture{ other.texture } { other.texture = nullptr; }
    Texture& operator=(Texture&& other) noexcept { clear(); texture = other.texture; other.texture = nullptr; return *this; }

    void init(int width, int height, const std::uint8_t* data) noexcept;
    void clear() noexcept;
    ImTextureID get() noexcept { return texture; }
};

void Texture::init(int width, int height, const std::uint8_t* data) noexcept
{
    texture = ImGui_CreateTextureRGBA(width, height, data);
}

void Texture::clear() noexcept
{
    if (texture)
        ImGui_DestroyTexture(texture);
    texture = nullptr;
}

static std::unordered_map<std::string, Texture> iconTextures;

ImTextureID SkinChanger::getItemIconTexture(const std::string& iconpath) noexcept
{
    if (iconpath.empty())
        return 0;

    if (iconTextures[iconpath].get())
        return iconTextures[iconpath].get();

    if (iconTextures.size() >= 50)
        iconTextures.erase(iconTextures.begin());

    if (const auto handle = interfaces->baseFileSystem->open(("resource/flash/" + iconpath + "_large.png").c_str(), "r", "GAME")) {
        if (const auto size = interfaces->baseFileSystem->size(handle); size >= 0) {
            const auto buffer = std::make_unique<std::uint8_t[]>(size);
            if (interfaces->baseFileSystem->read(buffer.get(), size, handle) > 0) {
                int width, height;
                stbi_set_flip_vertically_on_load_thread(false);

                if (const auto data = stbi_load_from_memory((const stbi_uc*)buffer.get(), size, &width, &height, nullptr, STBI_rgb_alpha)) {
                    iconTextures[iconpath].init(width, height, data);
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

    return iconTextures[iconpath].get();
}

void SkinChanger::clearItemIconTextures() noexcept
{
    iconTextures.clear();
}

SkinChanger::PaintKit::PaintKit(int id, const std::string& name, int rarity) noexcept : id{ id }, name{ name }, rarity{ rarity }
{
    nameUpperCase = Helpers::toUpper(Helpers::toWideString(name));
}

SkinChanger::PaintKit::PaintKit(int id, std::string&& name, int rarity) noexcept : id{ id }, name{ std::move(name) }, rarity{ rarity }
{
    nameUpperCase = Helpers::toUpper(Helpers::toWideString(this->name));
}

SkinChanger::PaintKit::PaintKit(int id, std::wstring&& name, std::string&& iconPath, int rarity) noexcept : id{ id }, nameUpperCase{ std::move(name) }, iconPath{ std::move(iconPath) }, rarity{ rarity }
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}

SkinChanger::PaintKit::PaintKit(int id, std::wstring&& name, int rarity) noexcept : id{ id }, nameUpperCase{ std::move(name) }, rarity{ rarity }
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
