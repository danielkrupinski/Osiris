#pragma once

#include <type_traits>

template <typename HookContext>
class ConfigVariableChangeHandler {
public:
    explicit ConfigVariableChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    template <typename ConfigVariable>
    void onConfigVariableValueChanged(ConfigVariable::ValueType newValue) const noexcept
    {
        if constexpr (std::is_same_v<ConfigVariable, BombTimerEnabled>) {
            if (!newValue)
                hookContext.template make<BombTimer>().onDisable();
        } else if constexpr (std::is_same_v<ConfigVariable, DefusingAlertEnabled>) {
            if (!newValue)
                hookContext.template make<DefusingAlert>().onDisable();
        } else if constexpr (std::is_same_v<ConfigVariable, PostRoundTimerEnabled>) {
            if (!newValue)
                hookContext.template make<PostRoundTimer>().onDisable();
        } else if constexpr (std::is_same_v<ConfigVariable, BombBeepSoundVisualizationEnabled>) {
            if (newValue)
                hookContext.soundWatcher().template startWatching<BombBeepSound>();
            else
                hookContext.soundWatcher().template stopWatching<BombBeepSound>();
        } else if constexpr (std::is_same_v<ConfigVariable, BombDefuseSoundVisualizationEnabled>) {
            if (newValue)
                hookContext.soundWatcher().template startWatching<BombDefuseSound>();
            else
                hookContext.soundWatcher().template stopWatching<BombDefuseSound>();
        } else if constexpr (std::is_same_v<ConfigVariable, BombPlantSoundVisualizationEnabled>) {
            if (newValue)
                hookContext.soundWatcher().template startWatching<BombPlantSound>();
            else
                hookContext.soundWatcher().template stopWatching<BombPlantSound>();
        } else if constexpr (std::is_same_v<ConfigVariable, FootstepSoundVisualizationEnabled>) {
            if (newValue)
                hookContext.soundWatcher().template startWatching<FootstepSound>();
            else
                hookContext.soundWatcher().template stopWatching<FootstepSound>();
        } else if constexpr (std::is_same_v<ConfigVariable, WeaponReloadSoundVisualizationEnabled>) {
            if (newValue)
                hookContext.soundWatcher().template startWatching<WeaponReloadSound>();
            else
                hookContext.soundWatcher().template stopWatching<WeaponReloadSound>();
        } else if constexpr (std::is_same_v<ConfigVariable, WeaponScopeSoundVisualizationEnabled>) {
            if (newValue)
                hookContext.soundWatcher().template startWatching<WeaponScopeSound>();
            else
                hookContext.soundWatcher().template stopWatching<WeaponScopeSound>();
        } else if constexpr (std::is_same_v<ConfigVariable, ModelGlowEnabled>) {
            if (!newValue)
                hookContext.featuresStates().visualFeaturesStates.modelGlowState.modelGlowDisabling = true;
        } else if constexpr (std::is_same_v<ConfigVariable, DefuseKitModelGlowEnabled>) {
            if (!newValue)
                hookContext.featuresStates().visualFeaturesStates.modelGlowState.defuseKitModelGlowDisabling = true;
        } else if constexpr (std::is_same_v<ConfigVariable, DroppedBombModelGlowEnabled>) {
            if (!newValue)
                hookContext.featuresStates().visualFeaturesStates.modelGlowState.droppedBombModelGlowDisabling = true;
        } else if constexpr (std::is_same_v<ConfigVariable, GrenadeProjectileModelGlowEnabled>) {
            if (!newValue)
                hookContext.featuresStates().visualFeaturesStates.modelGlowState.grenadeProjectileModelGlowDisabling = true;
        } else if constexpr (std::is_same_v<ConfigVariable, PlayerModelGlowEnabled>) {
            if (!newValue)
                hookContext.featuresStates().visualFeaturesStates.modelGlowState.playerModelGlowDisabling = true;
        } else if constexpr (std::is_same_v<ConfigVariable, TickingBombModelGlowEnabled>) {
            if (!newValue)
                hookContext.featuresStates().visualFeaturesStates.modelGlowState.tickingBombModelGlowDisabling = true;
        } else if constexpr (std::is_same_v<ConfigVariable, WeaponModelGlowEnabled>) {
            if (!newValue)
                hookContext.featuresStates().visualFeaturesStates.modelGlowState.weaponModelGlowDisabling = true;
        } else if constexpr (std::is_same_v<ConfigVariable, OutlineGlowEnabled>) {
            if (!newValue)
                hookContext.template make<GlowSceneObjects>().clearObjects();
        } else if constexpr (std::is_same_v<ConfigVariable, PlayerInfoInWorldBombDefuseIconEnabled>) {
            if (newValue)
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<DefuseIconPanel>();
            else
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<DefuseIconPanel>();
        } else if constexpr (std::is_same_v<ConfigVariable, PlayerInfoInWorldHostagePickupIconEnabled>) {
            if (newValue)
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<HostagePickupPanel>();
            else
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<HostagePickupPanel>();
        } else if constexpr (std::is_same_v<ConfigVariable, PlayerInfoInWorldHostageRescueIconEnabled>) {
            if (newValue)
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<HostageRescuePanel>();
            else
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<HostageRescuePanel>();
        } else if constexpr (std::is_same_v<ConfigVariable, PlayerInfoInWorldBlindedIconEnabled>) {
            if (newValue)
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<BlindedIconPanel>();
            else
                hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<BlindedIconPanel>();
        }
    }

private:
    HookContext& hookContext;
};
