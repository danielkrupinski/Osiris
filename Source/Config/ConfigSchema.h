#pragma once

#include <algorithm>

#include "ConfigVariableTypes.h"

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
        hudObject(configConversion);
        visualsObject(configConversion);
        soundObject(configConversion);
        return configConversion.endRoot();
    }

private:
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

        configConversion.endObject();
    }

    void visualsObject(auto&& configConversion)
    {
        configConversion.beginObject(u8"Visuals");

        configConversion.beginObject(u8"ModelGlow");
        configConversion.boolean(u8"Enabled", loadVariable<ModelGlowEnabled>(), saveVariable<ModelGlowEnabled>());
        configConversion.beginObject(u8"Players");
        configConversion.boolean(u8"Enabled", loadVariable<PlayerModelGlowEnabled>(), saveVariable<PlayerModelGlowEnabled>());
        configConversion.boolean(u8"OnlyEnemies", loadVariable<PlayerModelGlowOnlyEnemies>(), saveVariable<PlayerModelGlowOnlyEnemies>());
        configConversion.uint(u8"ColorMode", loadVariable<PlayerModelGlowColorMode>(), saveVariable<PlayerModelGlowColorMode>());
        configConversion.endObject();
        configConversion.boolean(u8"Weapons", loadVariable<WeaponModelGlowEnabled>(), saveVariable<WeaponModelGlowEnabled>());
        configConversion.boolean(u8"DroppedBomb", loadVariable<DroppedBombModelGlowEnabled>(), saveVariable<DroppedBombModelGlowEnabled>());
        configConversion.boolean(u8"TickingBomb", loadVariable<TickingBombModelGlowEnabled>(), saveVariable<TickingBombModelGlowEnabled>());
        configConversion.boolean(u8"DefuseKits", loadVariable<DefuseKitModelGlowEnabled>(), saveVariable<DefuseKitModelGlowEnabled>());
        configConversion.boolean(u8"GrenadeProjectiles", loadVariable<GrenadeProjectileModelGlowEnabled>(), saveVariable<GrenadeProjectileModelGlowEnabled>());
        configConversion.beginObject(u8"Hues");
        configConversion.uint(u8"PlayerBlue", loadVariable<PlayerModelGlowPlayerBlueHue>(), saveVariable<PlayerModelGlowPlayerBlueHue>());
        configConversion.uint(u8"PlayerGreen", loadVariable<PlayerModelGlowPlayerGreenHue>(), saveVariable<PlayerModelGlowPlayerGreenHue>());
        configConversion.uint(u8"PlayerYellow", loadVariable<PlayerModelGlowPlayerYellowHue>(), saveVariable<PlayerModelGlowPlayerYellowHue>());
        configConversion.uint(u8"PlayerOrange", loadVariable<PlayerModelGlowPlayerOrangeHue>(), saveVariable<PlayerModelGlowPlayerOrangeHue>());
        configConversion.uint(u8"PlayerPurple", loadVariable<PlayerModelGlowPlayerPurpleHue>(), saveVariable<PlayerModelGlowPlayerPurpleHue>());
        configConversion.uint(u8"TeamT", loadVariable<PlayerModelGlowTeamTHue>(), saveVariable<PlayerModelGlowTeamTHue>());
        configConversion.uint(u8"TeamCT", loadVariable<PlayerModelGlowTeamCTHue>(), saveVariable<PlayerModelGlowTeamCTHue>());
        configConversion.uint(u8"LowHealth", loadVariable<PlayerModelGlowLowHealthHue>(), saveVariable<PlayerModelGlowLowHealthHue>());
        configConversion.uint(u8"HighHealth", loadVariable<PlayerModelGlowHighHealthHue>(), saveVariable<PlayerModelGlowHighHealthHue>());
        configConversion.uint(u8"Enemy", loadVariable<PlayerModelGlowEnemyHue>(), saveVariable<PlayerModelGlowEnemyHue>());
        configConversion.uint(u8"Ally", loadVariable<PlayerModelGlowAllyHue>(), saveVariable<PlayerModelGlowAllyHue>());
        configConversion.endObject();
        configConversion.endObject();

        configConversion.beginObject(u8"OutlineGlow");
        configConversion.boolean(u8"Enabled", loadVariable<OutlineGlowEnabled>(), saveVariable<OutlineGlowEnabled>());
        configConversion.beginObject(u8"Players");
        configConversion.boolean(u8"Enabled", loadVariable<PlayerOutlineGlowEnabled>(), saveVariable<PlayerOutlineGlowEnabled>());
        configConversion.boolean(u8"OnlyEnemies", loadVariable<PlayerOutlineGlowOnlyEnemies>(), saveVariable<PlayerOutlineGlowOnlyEnemies>());
        configConversion.uint(u8"ColorMode", loadVariable<PlayerOutlineGlowColorMode>(), saveVariable<PlayerOutlineGlowColorMode>());
        configConversion.endObject();
        configConversion.boolean(u8"Weapons", loadVariable<WeaponOutlineGlowEnabled>(), saveVariable<WeaponOutlineGlowEnabled>());
        configConversion.boolean(u8"DroppedBomb", loadVariable<DroppedBombOutlineGlowEnabled>(), saveVariable<DroppedBombOutlineGlowEnabled>());
        configConversion.boolean(u8"TickingBomb", loadVariable<TickingBombOutlineGlowEnabled>(), saveVariable<TickingBombOutlineGlowEnabled>());
        configConversion.boolean(u8"DefuseKits", loadVariable<DefuseKitOutlineGlowEnabled>(), saveVariable<DefuseKitOutlineGlowEnabled>());
        configConversion.boolean(u8"Hostages", loadVariable<HostageOutlineGlowEnabled>(), saveVariable<HostageOutlineGlowEnabled>());
        configConversion.boolean(u8"GrenadeProjectiles", loadVariable<GrenadeProjectileOutlineGlowEnabled>(), saveVariable<GrenadeProjectileOutlineGlowEnabled>());
        configConversion.endObject();

        configConversion.beginObject(u8"PlayerInfoInWorld");
        configConversion.boolean(u8"Enabled", loadVariable<PlayerInfoInWorldEnabled>(), saveVariable<PlayerInfoInWorldEnabled>());
        configConversion.boolean(u8"OnlyEnemies", loadVariable<PlayerInfoInWorldOnlyEnemies>(), saveVariable<PlayerInfoInWorldOnlyEnemies>());

        configConversion.beginObject(u8"PlayerPositionArrow");
        configConversion.boolean(u8"Enabled", loadVariable<PlayerInfoInWorldPlayerPositionArrowEnabled>(), saveVariable<PlayerInfoInWorldPlayerPositionArrowEnabled>());
        configConversion.uint(u8"ColorMode", loadVariable<PlayerInfoInWorldPlayerPositionArrowColorMode>(), saveVariable<PlayerInfoInWorldPlayerPositionArrowColorMode>());
        configConversion.endObject();

        configConversion.beginObject(u8"Health");
        configConversion.boolean(u8"Enabled", loadVariable<PlayerInfoInWorldPlayerHealthEnabled>(), saveVariable<PlayerInfoInWorldPlayerHealthEnabled>());
        configConversion.uint(u8"ColorMode", loadVariable<PlayerInfoInWorldPlayerHealthColorMode>(), saveVariable<PlayerInfoInWorldPlayerHealthColorMode>());
        configConversion.endObject();

        configConversion.boolean(u8"ActiveWeaponIcon", loadVariable<PlayerInfoInWorldActiveWeaponIconEnabled>(), saveVariable<PlayerInfoInWorldActiveWeaponIconEnabled>());
        configConversion.boolean(u8"BombCarrierIcon", loadVariable<PlayerInfoInWorldBombCarrierIconEnabled>(), saveVariable<PlayerInfoInWorldBombCarrierIconEnabled>());
        configConversion.boolean(u8"BombPlantIcon", loadVariable<PlayerInfoInWorldBombPlantIconEnabled>(), saveVariable<PlayerInfoInWorldBombPlantIconEnabled>());
        configConversion.boolean(u8"ActiveWeaponAmmo", loadVariable<PlayerInfoInWorldActiveWeaponAmmoEnabled>(), saveVariable<PlayerInfoInWorldActiveWeaponAmmoEnabled>());
        configConversion.boolean(u8"BombDefuseIcon", loadVariable<PlayerInfoInWorldBombDefuseIconEnabled>(), saveVariable<PlayerInfoInWorldBombDefuseIconEnabled>());
        configConversion.boolean(u8"HostagePickupIcon", loadVariable<PlayerInfoInWorldHostagePickupIconEnabled>(), saveVariable<PlayerInfoInWorldHostagePickupIconEnabled>());
        configConversion.boolean(u8"HostageRescueIcon", loadVariable<PlayerInfoInWorldHostageRescueIconEnabled>(), saveVariable<PlayerInfoInWorldHostageRescueIconEnabled>());
        configConversion.boolean(u8"BlindedIcon", loadVariable<PlayerInfoInWorldBlindedIconEnabled>(), saveVariable<PlayerInfoInWorldBlindedIconEnabled>());
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
                return static_cast<typename ConfigVariable::ValueType::ValueType>(hookContext.config().template getVariable<ConfigVariable>());
            };
        } else if constexpr (std::is_enum_v<typename ConfigVariable::ValueType>) {
            return [this] {
                return static_cast<std::underlying_type_t<typename ConfigVariable::ValueType>>(hookContext.config().template getVariable<ConfigVariable>());
            };
        } else {
            return [this] {
                return hookContext.config().template getVariable<ConfigVariable>();
            };
        }
    }

    HookContext& hookContext;
};
