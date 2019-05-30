#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "SkinChanger.h"
#include "../Config.h"

#include "../nSkinz/config_.hpp"
#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/Engine.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/EntityList.h"
#include "../SDK/Entity.h"
#include "../nSkinz/Utilities/vmt_smart_hook.hpp"

enum class EStickerAttributeType
{
    Index,
    Wear,
    Scale,
    Rotation
};

static auto s_econ_item_interface_wrapper_offset = std::uint16_t(0);

struct GetStickerAttributeBySlotIndexFloat
{
    static auto __fastcall hooked(void* thisptr, void*, const int slot,
        const EStickerAttributeType attribute, const float unknown) -> float
    {
        auto item = reinterpret_cast<Entity*>(std::uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

        const auto defindex = item->GetItemDefinitionIndex();

        auto config = g_config.get_by_definition_index(defindex);

        if (config)
        {
            switch (attribute)
            {
            case EStickerAttributeType::Wear:
                return config->stickers.at(slot).wear;
            case EStickerAttributeType::Scale:
                return config->stickers.at(slot).scale;
            case EStickerAttributeType::Rotation:
                return config->stickers.at(slot).rotation;
            default:
                break;
            }
        }

        return m_original(thisptr, nullptr, slot, attribute, unknown);
    }

    static decltype(&hooked) m_original;
};

decltype(GetStickerAttributeBySlotIndexFloat::m_original) GetStickerAttributeBySlotIndexFloat::m_original;

struct GetStickerAttributeBySlotIndexInt
{
    static auto __fastcall hooked(void* thisptr, void*, const int slot,
        const EStickerAttributeType attribute, const int unknown) -> int
    {
        auto item = reinterpret_cast<Entity*>(std::uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

        if (attribute == EStickerAttributeType::Index)
        {
            const auto defindex = item->GetItemDefinitionIndex();

            auto config = g_config.get_by_definition_index(defindex);

            if (config)
                return config->stickers.at(slot).kit;
        }

        return m_original(thisptr, nullptr, slot, attribute, unknown);
    }

    static decltype(&hooked) m_original;
};

decltype(GetStickerAttributeBySlotIndexInt::m_original) GetStickerAttributeBySlotIndexInt::m_original;

auto apply_sticker_changer(Entity* item) -> void
{
    if (!s_econ_item_interface_wrapper_offset)
        s_econ_item_interface_wrapper_offset = netvarManager.get_offset(FNV("CBaseAttributableItem->m_Item")) + 0xC;

    static vmt_multi_hook hook;

    const auto econ_item_interface_wrapper = std::uintptr_t(item) + s_econ_item_interface_wrapper_offset;

    if (hook.initialize_and_hook_instance(reinterpret_cast<void*>(econ_item_interface_wrapper)))
    {
        hook.apply_hook<GetStickerAttributeBySlotIndexFloat>(4);
        hook.apply_hook<GetStickerAttributeBySlotIndexInt>(5);
    }
}

static auto erase_override_if_exists_by_index(const int definition_index) -> void
{
    // We have info about the item not needed to be overridden
    if (const auto original_item = game_data::get_weapon_info(definition_index))
    {
        auto& icon_override_map = g_config.get_icon_override_map();

        if (!original_item->icon)
            return;

        const auto override_entry = icon_override_map.find(original_item->icon);

        // We are overriding its icon when not needed
        if (override_entry != end(icon_override_map))
            icon_override_map.erase(override_entry); // Remove the leftover override
    }
}

static auto apply_config_on_attributable_item(Entity* item, const item_setting* config,
    const unsigned xuid_low) -> void
{
    // Force fallback values to be used.
    item->GetItemIDHigh() = -1;

    // Set the owner of the weapon to our lower XUID. (fixes StatTrak)
    item->GetAccountID() = xuid_low;

    if (config->entity_quality_index)
        item->GetEntityQuality() = config->entity_quality_index;

    if (config->custom_name[0])
        strcpy_s(item->GetCustomName(), config->custom_name);

    if (config->paint_kit_index)
        item->GetFallbackPaintKit() = config->paint_kit_index;

    if (config->seed)
        item->GetFallbackSeed() = config->seed;

    if (config->stat_trak)
        item->GetFallbackStatTrak() = config->stat_trak;

    item->GetFallbackWear() = config->wear;

    auto& definition_index = item->GetItemDefinitionIndex();

    auto& icon_override_map = g_config.get_icon_override_map();

    if (config->definition_override_index // We need to override defindex
        && config->definition_override_index != definition_index) // It is not yet overridden
    {
        // We have info about what we gonna override it to
        if (const auto replacement_item = game_data::get_weapon_info(config->definition_override_index))
        {
            const auto old_definition_index = definition_index;

            definition_index = config->definition_override_index;

            // Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
            //item->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(config->definition_override_index).model);
            item->setModelIndex(interfaces.modelInfo->getModelIndex(replacement_item->model));
            item->preDataUpdate(0);

            // We didn't override 0, but some actual weapon, that we have data for
            if (old_definition_index)
            {
                if (const auto original_item = game_data::get_weapon_info(old_definition_index))
                {
                    if (original_item->icon && replacement_item->icon)
                        icon_override_map[original_item->icon] = replacement_item->icon;
                }
            }
        }
    } else
    {
        erase_override_if_exists_by_index(definition_index);
    }

    apply_sticker_changer(item);
}

static auto get_wearable_create_fn() noexcept
{
    auto clazz = interfaces.client->getAllClasses();

    // Please, if you gonna paste it into a cheat use classids here. I use names because they
    // won't change in the foreseeable future and i dont need high speed, but chances are
    // you already have classids, so use them instead, they are faster.
    while (fnv::hash_runtime(clazz->networkName) != FNV("CEconWearable"))
        clazz = clazz->next;

    return clazz->createFunction;
}

static auto make_glove(int entry, int serial) -> Entity*
{
    static auto create_wearable_fn = get_wearable_create_fn();

    create_wearable_fn(entry, serial);

    const auto glove = interfaces.entityList->getEntity(entry);

    // He he
    {
        static auto set_abs_origin_addr = platform::find_pattern("client_panorama", "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1", "xxxxxxxxxxxx");

        const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(set_abs_origin_addr);

        static constexpr Vector new_pos = { 10000.f, 10000.f, 10000.f };

        set_abs_origin_fn(glove, new_pos);
    }

    return glove;
}

static auto post_data_update_start(Entity* local) -> void
{
    const auto local_index = local->index();

    /*if(auto player_resource = *g_player_resource)
    {
        player_resource->GetCoins()[local_index] = 890;
        player_resource->GetMusicKits()[local_index] = 3;
        player_resource->GetRanks()[local_index] = 1;
        player_resource->GetWins()[local_index] = 1337;
    }*/

    PlayerInfo player_info;
    if (!interfaces.engine->getPlayerInfo(local_index, player_info))
        return;

    // Handle glove config
    {
        const auto wearables = local->GetWearables();

        const auto glove_config = g_config.get_by_definition_index(GLOVE_T_SIDE);

        static int glove_handle;

        auto glove = interfaces.entityList->getEntityFromHandle(wearables[0]);

        if (!glove) // There is no glove
        {
            // Try to get our last created glove
            const auto our_glove = interfaces.entityList->getEntityFromHandle(glove_handle);

            if (our_glove) // Our glove still exists
            {
                wearables[0] = glove_handle;
                glove = our_glove;
            }
        }

        if (!local->isAlive())
        {
            // We are dead but we have a glove, destroy it
            if (glove)
            {
                glove->setDestroyedOnRecreateEntities();
                glove->release();
            }

            return;
        }

        if (glove_config && glove_config->definition_override_index)
        {
            // We don't have a glove, but we should
            if (!glove)
            {
                const auto entry = interfaces.entityList->getHighestEntityIndex() + 1;
                const auto serial = rand() % 0x1000;

                glove = make_glove(entry, serial);

                wearables[0] = entry | serial << 16;

                // Let's store it in case we somehow lose it.
                glove_handle = wearables[0];
            }

            // Thanks, Beakers
            glove->index() = -1;

            apply_config_on_attributable_item(glove, glove_config, player_info.xuidLow);
        }
    }

    // Handle weapon configs
    {
        auto& weapons = local->GetWeapons();

        for (auto weapon_handle : weapons)
        {
            if (weapon_handle == -1)
                break;

            auto weapon = interfaces.entityList->getEntityFromHandle(weapon_handle);

            if (!weapon)
                continue;

            auto& definition_index = weapon->GetItemDefinitionIndex();

            // All knives are terrorist knives.
            if (const auto active_conf = g_config.get_by_definition_index(is_knife(definition_index) ? WEAPON_KNIFE : definition_index))
                apply_config_on_attributable_item(weapon, active_conf, player_info.xuidLow);
            else
                erase_override_if_exists_by_index(definition_index);
        }
    }

    const auto view_model = interfaces.entityList->getEntityFromHandle(local->GetViewModel());

    if (!view_model)
        return;

    const auto view_model_weapon = interfaces.entityList->getEntityFromHandle(view_model->GetWeapon());

    if (!view_model_weapon)
        return;

    const auto override_info = game_data::get_weapon_info(view_model_weapon->GetItemDefinitionIndex());

    if (!override_info)
        return;

    const auto override_model_index = interfaces.modelInfo->getModelIndex(override_info->model);
    view_model->GetModelIndex() = override_model_index;

    const auto world_model = interfaces.entityList->getEntityFromHandle(view_model_weapon->GetWeaponWorldModel());

    if (!world_model)
        return;

    world_model->GetModelIndex() = override_model_index + 1;
}

static bool hudUpdateRequired{ false };

static constexpr void updateHud() noexcept
{
    if (auto hud_weapons = memory.findHudElement(memory.hud, "CCSGO_HudWeaponSelection") - 0x28) {
        for (int i = 0; i < *(hud_weapons + 0x20); i++)
            i = memory.clearHudWeapon(hud_weapons, i);
    }
    hudUpdateRequired = false;
}

void SkinChanger::run(FrameStage stage) noexcept
{
    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        if (const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()); localPlayer && localPlayer->isAlive()) {
            post_data_update_start(localPlayer);
            if (hudUpdateRequired && !localPlayer->isDormant())
                updateHud();
        }
    }
}

void SkinChanger::scheduleHudUpdate() noexcept
{
    hudUpdateRequired = true;
}
