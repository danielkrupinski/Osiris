#pragma once

#include <algorithm>

#include "ConfigVariableTypes.h"
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
class ConfigSchema {
public:
    explicit ConfigSchema(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) performConversion(auto&& configConversion)
    {
        configConversion.beginRoot();
        combatObject(configConversion);
        hudObject(configConversion);
        visualsObject(configConversion);
        soundObject(configConversion);
        return configConversion.endRoot();
    }

private:
    void combatObject(auto&& configConversion)
    {
        configConversion.beginObject(u8"Combat");

        configConversion.beginObject(u8"NoScopeInaccuracyVis");
        configConversion.boolean(u8"Enabled", loadVariable<no_scope_inaccuracy_vis_vars::Enabled>(), saveVariable<no_scope_inaccuracy_vis_vars::Enabled>());
        configConversion.endObject();

        configConversion.endObject();
    }

    void hudObject(auto&& configConversion)
    {
        configConversion.beginObject(u8"Hud");

        configConversion.beginObject(u8"BombTimer");
        configConversion.boolean(u8"Enabled", loadVariable<BombTimerEnabled>(), saveVariable<BombTimerEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"BombDefuseAlert");
        configConversion.boolean(u8"Enabled", loadVariable<DefusingAlertEnabled>(), saveVariable<DefusingAlertEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"PreserveKillfeed");
        configConversion.boolean(u8"Enabled", loadVariable<KillfeedPreserverEnabled>(), saveVariable<KillfeedPreserverEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"PostRoundTimer");
        configConversion.boolean(u8"Enabled", loadVariable<PostRoundTimerEnabled>(), saveVariable<PostRoundTimerEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"BombPlantAlert");
        configConversion.boolean(u8"Enabled", loadVariable<BombPlantAlertEnabled>(), saveVariable<BombPlantAlertEnabled>());
        configConversion.endObject();

        configConversion.endObject();
    }

    void visualsObject(auto&& configConversion)
    {
        configConversion.beginObject(u8"Visuals");

        configConversion.beginObject(u8"ModelGlow");
        configConversion.boolean(u8"Enabled", loadVariable<model_glow_vars::Enabled>(), saveVariable<model_glow_vars::Enabled>());
        configConversion.beginObject(u8"Players");
        configConversion.boolean(u8"Enabled", loadVariable<model_glow_vars::GlowPlayers>(), saveVariable<model_glow_vars::GlowPlayers>());
        configConversion.boolean(u8"OnlyEnemies", loadVariable<model_glow_vars::GlowOnlyEnemies>(), saveVariable<model_glow_vars::GlowOnlyEnemies>());
        configConversion.uint(u8"ColorMode", loadVariable<model_glow_vars::PlayerGlowColorMode>(), saveVariable<model_glow_vars::PlayerGlowColorMode>());
        configConversion.endObject();
        configConversion.boolean(u8"Weapons", loadVariable<model_glow_vars::GlowWeapons>(), saveVariable<model_glow_vars::GlowWeapons>());
        configConversion.boolean(u8"DroppedBomb", loadVariable<model_glow_vars::GlowDroppedBomb>(), saveVariable<model_glow_vars::GlowDroppedBomb>());
        configConversion.boolean(u8"TickingBomb", loadVariable<model_glow_vars::GlowTickingBomb>(), saveVariable<model_glow_vars::GlowTickingBomb>());
        configConversion.boolean(u8"DefuseKits", loadVariable<model_glow_vars::GlowDefuseKits>(), saveVariable<model_glow_vars::GlowDefuseKits>());
        configConversion.boolean(u8"GrenadeProjectiles", loadVariable<model_glow_vars::GlowGrenadeProjectiles>(), saveVariable<model_glow_vars::GlowGrenadeProjectiles>());
        configConversion.beginObject(u8"Hues");
        configConversion.uint(u8"PlayerBlue", loadVariable<model_glow_vars::PlayerBlueHue>(), saveVariable<model_glow_vars::PlayerBlueHue>());
        configConversion.uint(u8"PlayerGreen", loadVariable<model_glow_vars::PlayerGreenHue>(), saveVariable<model_glow_vars::PlayerGreenHue>());
        configConversion.uint(u8"PlayerYellow", loadVariable<model_glow_vars::PlayerYellowHue>(), saveVariable<model_glow_vars::PlayerYellowHue>());
        configConversion.uint(u8"PlayerOrange", loadVariable<model_glow_vars::PlayerOrangeHue>(), saveVariable<model_glow_vars::PlayerOrangeHue>());
        configConversion.uint(u8"PlayerPurple", loadVariable<model_glow_vars::PlayerPurpleHue>(), saveVariable<model_glow_vars::PlayerPurpleHue>());
        configConversion.uint(u8"TeamT", loadVariable<model_glow_vars::TeamTHue>(), saveVariable<model_glow_vars::TeamTHue>());
        configConversion.uint(u8"TeamCT", loadVariable<model_glow_vars::TeamCTHue>(), saveVariable<model_glow_vars::TeamCTHue>());
        configConversion.uint(u8"LowHealth", loadVariable<model_glow_vars::LowHealthHue>(), saveVariable<model_glow_vars::LowHealthHue>());
        configConversion.uint(u8"HighHealth", loadVariable<model_glow_vars::HighHealthHue>(), saveVariable<model_glow_vars::HighHealthHue>());
        configConversion.uint(u8"Enemy", loadVariable<model_glow_vars::EnemyHue>(), saveVariable<model_glow_vars::EnemyHue>());
        configConversion.uint(u8"Ally", loadVariable<model_glow_vars::AllyHue>(), saveVariable<model_glow_vars::AllyHue>());
        configConversion.uint(u8"Molotov", loadVariable<model_glow_vars::MolotovHue>(), saveVariable<model_glow_vars::MolotovHue>());
        configConversion.uint(u8"Flashbang", loadVariable<model_glow_vars::FlashbangHue>(), saveVariable<model_glow_vars::FlashbangHue>());
        configConversion.uint(u8"HEGrenade", loadVariable<model_glow_vars::HEGrenadeHue>(), saveVariable<model_glow_vars::HEGrenadeHue>());
        configConversion.uint(u8"SmokeGrenade", loadVariable<model_glow_vars::SmokeGrenadeHue>(), saveVariable<model_glow_vars::SmokeGrenadeHue>());
        configConversion.uint(u8"DroppedBomb", loadVariable<model_glow_vars::DroppedBombHue>(), saveVariable<model_glow_vars::DroppedBombHue>());
        configConversion.uint(u8"TickingBomb", loadVariable<model_glow_vars::TickingBombHue>(), saveVariable<model_glow_vars::TickingBombHue>());
        configConversion.uint(u8"DefuseKit", loadVariable<model_glow_vars::DefuseKitHue>(), saveVariable<model_glow_vars::DefuseKitHue>());
        configConversion.endObject();
        configConversion.endObject();

        configConversion.beginObject(u8"OutlineGlow");
        configConversion.boolean(u8"Enabled", loadVariable<outline_glow_vars::Enabled>(), saveVariable<outline_glow_vars::Enabled>());
        configConversion.beginObject(u8"Players");
        configConversion.boolean(u8"Enabled", loadVariable<outline_glow_vars::GlowPlayers>(), saveVariable<outline_glow_vars::GlowPlayers>());
        configConversion.boolean(u8"OnlyEnemies", loadVariable<outline_glow_vars::GlowOnlyEnemies>(), saveVariable<outline_glow_vars::GlowOnlyEnemies>());
        configConversion.uint(u8"ColorMode", loadVariable<outline_glow_vars::PlayerGlowColorMode>(), saveVariable<outline_glow_vars::PlayerGlowColorMode>());
        configConversion.endObject();
        configConversion.boolean(u8"Weapons", loadVariable<outline_glow_vars::GlowWeapons>(), saveVariable<outline_glow_vars::GlowWeapons>());
        configConversion.boolean(u8"DroppedBomb", loadVariable<outline_glow_vars::GlowDroppedBomb>(), saveVariable<outline_glow_vars::GlowDroppedBomb>());
        configConversion.boolean(u8"TickingBomb", loadVariable<outline_glow_vars::GlowTickingBomb>(), saveVariable<outline_glow_vars::GlowTickingBomb>());
        configConversion.boolean(u8"DefuseKits", loadVariable<outline_glow_vars::GlowDefuseKits>(), saveVariable<outline_glow_vars::GlowDefuseKits>());
        configConversion.boolean(u8"Hostages", loadVariable<outline_glow_vars::GlowHostages>(), saveVariable<outline_glow_vars::GlowHostages>());
        configConversion.boolean(u8"GrenadeProjectiles", loadVariable<outline_glow_vars::GlowGrenadeProjectiles>(), saveVariable<outline_glow_vars::GlowGrenadeProjectiles>());
        configConversion.beginObject(u8"Hues");
        configConversion.uint(u8"PlayerBlue", loadVariable<outline_glow_vars::PlayerBlueHue>(), saveVariable<outline_glow_vars::PlayerBlueHue>());
        configConversion.uint(u8"PlayerGreen", loadVariable<outline_glow_vars::PlayerGreenHue>(), saveVariable<outline_glow_vars::PlayerGreenHue>());
        configConversion.uint(u8"PlayerYellow", loadVariable<outline_glow_vars::PlayerYellowHue>(), saveVariable<outline_glow_vars::PlayerYellowHue>());
        configConversion.uint(u8"PlayerOrange", loadVariable<outline_glow_vars::PlayerOrangeHue>(), saveVariable<outline_glow_vars::PlayerOrangeHue>());
        configConversion.uint(u8"PlayerPurple", loadVariable<outline_glow_vars::PlayerPurpleHue>(), saveVariable<outline_glow_vars::PlayerPurpleHue>());
        configConversion.uint(u8"TeamT", loadVariable<outline_glow_vars::TeamTHue>(), saveVariable<outline_glow_vars::TeamTHue>());
        configConversion.uint(u8"TeamCT", loadVariable<outline_glow_vars::TeamCTHue>(), saveVariable<outline_glow_vars::TeamCTHue>());
        configConversion.uint(u8"LowHealth", loadVariable<outline_glow_vars::LowHealthHue>(), saveVariable<outline_glow_vars::LowHealthHue>());
        configConversion.uint(u8"HighHealth", loadVariable<outline_glow_vars::HighHealthHue>(), saveVariable<outline_glow_vars::HighHealthHue>());
        configConversion.uint(u8"Enemy", loadVariable<outline_glow_vars::EnemyHue>(), saveVariable<outline_glow_vars::EnemyHue>());
        configConversion.uint(u8"Ally", loadVariable<outline_glow_vars::AllyHue>(), saveVariable<outline_glow_vars::AllyHue>());
        configConversion.uint(u8"Molotov", loadVariable<outline_glow_vars::MolotovHue>(), saveVariable<outline_glow_vars::MolotovHue>());
        configConversion.uint(u8"Flashbang", loadVariable<outline_glow_vars::FlashbangHue>(), saveVariable<outline_glow_vars::FlashbangHue>());
        configConversion.uint(u8"HEGrenade", loadVariable<outline_glow_vars::HEGrenadeHue>(), saveVariable<outline_glow_vars::HEGrenadeHue>());
        configConversion.uint(u8"SmokeGrenade", loadVariable<outline_glow_vars::SmokeGrenadeHue>(), saveVariable<outline_glow_vars::SmokeGrenadeHue>());
        configConversion.uint(u8"DroppedBomb", loadVariable<outline_glow_vars::DroppedBombHue>(), saveVariable<outline_glow_vars::DroppedBombHue>());
        configConversion.uint(u8"TickingBomb", loadVariable<outline_glow_vars::TickingBombHue>(), saveVariable<outline_glow_vars::TickingBombHue>());
        configConversion.uint(u8"DefuseKit", loadVariable<outline_glow_vars::DefuseKitHue>(), saveVariable<outline_glow_vars::DefuseKitHue>());
        configConversion.uint(u8"Hostage", loadVariable<outline_glow_vars::HostageHue>(), saveVariable<outline_glow_vars::HostageHue>());
        configConversion.endObject();
        configConversion.endObject();

        configConversion.beginObject(u8"PlayerInfoInWorld");
        configConversion.boolean(u8"Enabled", loadVariable<player_info_vars::Enabled>(), saveVariable<player_info_vars::Enabled>());
        configConversion.boolean(u8"OnlyEnemies", loadVariable<player_info_vars::OnlyEnemies>(), saveVariable<player_info_vars::OnlyEnemies>());

        configConversion.beginObject(u8"PlayerPositionArrow");
        configConversion.boolean(u8"Enabled", loadVariable<player_info_vars::PlayerPositionArrowEnabled>(), saveVariable<player_info_vars::PlayerPositionArrowEnabled>());
        configConversion.uint(u8"ColorMode", loadVariable<player_info_vars::PlayerPositionArrowColorMode>(), saveVariable<player_info_vars::PlayerPositionArrowColorMode>());
        configConversion.endObject();

        configConversion.beginObject(u8"Health");
        configConversion.boolean(u8"Enabled", loadVariable<player_info_vars::PlayerHealthEnabled>(), saveVariable<player_info_vars::PlayerHealthEnabled>());
        configConversion.uint(u8"ColorMode", loadVariable<player_info_vars::PlayerHealthColorMode>(), saveVariable<player_info_vars::PlayerHealthColorMode>());
        configConversion.endObject();

        configConversion.boolean(u8"ActiveWeaponIcon", loadVariable<player_info_vars::ActiveWeaponIconEnabled>(), saveVariable<player_info_vars::ActiveWeaponIconEnabled>());
        configConversion.boolean(u8"BombCarrierIcon", loadVariable<player_info_vars::BombCarrierIconEnabled>(), saveVariable<player_info_vars::BombCarrierIconEnabled>());
        configConversion.boolean(u8"BombPlantIcon", loadVariable<player_info_vars::BombPlantIconEnabled>(), saveVariable<player_info_vars::BombPlantIconEnabled>());
        configConversion.boolean(u8"ActiveWeaponAmmo", loadVariable<player_info_vars::ActiveWeaponAmmoEnabled>(), saveVariable<player_info_vars::ActiveWeaponAmmoEnabled>());
        configConversion.boolean(u8"BombDefuseIcon", loadVariable<player_info_vars::BombDefuseIconEnabled>(), saveVariable<player_info_vars::BombDefuseIconEnabled>());
        configConversion.boolean(u8"HostagePickupIcon", loadVariable<player_info_vars::HostagePickupIconEnabled>(), saveVariable<player_info_vars::HostagePickupIconEnabled>());
        configConversion.boolean(u8"HostageRescueIcon", loadVariable<player_info_vars::HostageRescueIconEnabled>(), saveVariable<player_info_vars::HostageRescueIconEnabled>());
        configConversion.boolean(u8"BlindedIcon", loadVariable<player_info_vars::BlindedIconEnabled>(), saveVariable<player_info_vars::BlindedIconEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"ViewmodelMod");
        configConversion.boolean(u8"Enabled", loadVariable<viewmodel_mod_vars::Enabled>(), saveVariable<viewmodel_mod_vars::Enabled>());
        configConversion.boolean(u8"ModifyFov", loadVariable<viewmodel_mod_vars::ModifyFov>(), saveVariable<viewmodel_mod_vars::ModifyFov>());
        configConversion.uint(u8"Fov", loadVariable<viewmodel_mod_vars::Fov>(), saveVariable<viewmodel_mod_vars::Fov>());
        configConversion.endObject();

        configConversion.endObject();
    }

    void soundObject(auto&& configConversion)
    {
        configConversion.beginObject(u8"Sound");
        configConversion.beginObject(u8"Visualizations");

        configConversion.beginObject(u8"Footsteps");
        configConversion.boolean(u8"Enabled", loadVariable<FootstepSoundVisualizationEnabled>(), saveVariable<FootstepSoundVisualizationEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"BombPlant");
        configConversion.boolean(u8"Enabled", loadVariable<BombPlantSoundVisualizationEnabled>(), saveVariable<BombPlantSoundVisualizationEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"BombBeep");
        configConversion.boolean(u8"Enabled", loadVariable<BombBeepSoundVisualizationEnabled>(), saveVariable<BombBeepSoundVisualizationEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"BombDefuse");
        configConversion.boolean(u8"Enabled", loadVariable<BombDefuseSoundVisualizationEnabled>(), saveVariable<BombDefuseSoundVisualizationEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"WeaponScope");
        configConversion.boolean(u8"Enabled", loadVariable<WeaponScopeSoundVisualizationEnabled>(), saveVariable<WeaponScopeSoundVisualizationEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"WeaponReload");
        configConversion.boolean(u8"Enabled", loadVariable<WeaponReloadSoundVisualizationEnabled>(), saveVariable<WeaponReloadSoundVisualizationEnabled>());
        configConversion.endObject();

        configConversion.endObject();
        configConversion.endObject();
    }

    template <std::unsigned_integral T>
    [[nodiscard]] static T saturateCast(std::unsigned_integral auto value) noexcept
    {
        if (value < (std::numeric_limits<T>::min)())
            return (std::numeric_limits<T>::min)();
        if (value > (std::numeric_limits<T>::max)())
            return (std::numeric_limits<T>::max)();
        return static_cast<T>(value);
    }

    template <typename ConfigVariable>
    [[nodiscard]] auto loadVariable()
    {
        if constexpr (IsRangeConstrained<typename ConfigVariable::ValueType>::value) {
            return [this](auto value) {
                if constexpr (std::is_same_v<color::HueInteger, typename ConfigVariable::ValueType::ValueType>) {
                    color::HueInteger hue{std::clamp(saturateCast<color::HueInteger::UnderlyingType>(value), color::HueInteger::kMin, color::HueInteger::kMax)};
                    hookContext.config().template setVariableWithoutAutoSave<ConfigVariable>(typename ConfigVariable::ValueType{std::clamp(hue, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax)});
                } else if constexpr (std::is_same_v<std::uint8_t, typename ConfigVariable::ValueType::ValueType>) {
                    hookContext.config().template setVariableWithoutAutoSave<ConfigVariable>(typename ConfigVariable::ValueType{std::clamp(saturateCast<std::uint8_t>(value), ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax)});
                } else {
                    static_assert(!std::is_same_v<ConfigVariable, ConfigVariable>, "Unsupported type");
                }
            };
        } else if constexpr (std::is_enum_v<typename ConfigVariable::ValueType>) {
            return [this](std::integral auto value) {
                hookContext.config().template setVariableWithoutAutoSave<ConfigVariable>(static_cast<ConfigVariable::ValueType>(value));
            };
        } else {
            return [this](ConfigVariable::ValueType value) {
                hookContext.config().template setVariableWithoutAutoSave<ConfigVariable>(value);
            };
        }
    }

    template <typename ConfigVariable>
    [[nodiscard]] auto saveVariable()
    {
        if constexpr (IsRangeConstrained<typename ConfigVariable::ValueType>::value) {
            return [this] {
                return static_cast<typename ConfigVariable::ValueType::ValueType>(GET_CONFIG_VAR(ConfigVariable));
            };
        } else if constexpr (std::is_enum_v<typename ConfigVariable::ValueType>) {
            return [this] {
                return static_cast<std::underlying_type_t<typename ConfigVariable::ValueType>>(GET_CONFIG_VAR(ConfigVariable));
            };
        } else {
            return [this] {
                return GET_CONFIG_VAR(ConfigVariable);
            };
        }
    }

    HookContext& hookContext;
};
