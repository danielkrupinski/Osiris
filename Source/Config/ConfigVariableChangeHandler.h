#pragma once

#include <type_traits>

#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVis.h>
#include <Features/Hud/BombPlantAlert/BombPlantAlert.h>
#include <Features/Hud/BombTimer/BombTimer.h>
#include <Features/Hud/DefusingAlert/DefusingAlert.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimer.h>
#include <Features/Sound/Details/BombBeepSound.h>
#include <Features/Sound/Details/BombDefuseSound.h>
#include <Features/Sound/Details/BombPlantSound.h>
#include <Features/Sound/Details/FootstepSound.h>
#include <Features/Sound/Details/WeaponReloadSound.h>
#include <Features/Sound/Details/WeaponScopeSound.h>
#include <Features/Visuals/ModelGlow/ModelGlowDeactivationFlags.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerStateIcons/PlayerStateIconsToShow.h>
#include <Hooks/ClientModeHooks.h>
#include <OutlineGlow/GlowSceneObjects.h>

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
        onConfigVariableValueChanged(newValue, std::type_identity<ConfigVariable>{});
    }

private:
    template <typename ConfigVariable>
    void onConfigVariableValueChanged(ConfigVariable::ValueType /* newValue */, std::type_identity<ConfigVariable>) const noexcept
    {
    }

    #define ON_CHANGE(ConfigVariable) \
    void onConfigVariableValueChanged(ConfigVariable::ValueType newValue, std::type_identity<ConfigVariable>) const noexcept

    ON_CHANGE(BombTimerEnabled)
    {
        if (newValue == false)
            hookContext.template make<BombTimer>().onDisable();
    }

    ON_CHANGE(DefusingAlertEnabled)
    {
        if (newValue == false)
            hookContext.template make<DefusingAlert>().onDisable();
    }

    ON_CHANGE(PostRoundTimerEnabled)
    {
        if (newValue == false)
            hookContext.template make<PostRoundTimer>().onDisable();
    }

    ON_CHANGE(BombBeepSoundVisualizationEnabled)
    {
        if (newValue == true)
            hookContext.soundWatcher().template startWatching<BombBeepSound>();
        else
            hookContext.soundWatcher().template stopWatching<BombBeepSound>();
    }

    ON_CHANGE(BombDefuseSoundVisualizationEnabled)
    {
        if (newValue == true)
            hookContext.soundWatcher().template startWatching<BombDefuseSound>();
        else
            hookContext.soundWatcher().template stopWatching<BombDefuseSound>();
    }

    ON_CHANGE(BombPlantSoundVisualizationEnabled)
    {
        if (newValue == true)
            hookContext.soundWatcher().template startWatching<BombPlantSound>();
        else
            hookContext.soundWatcher().template stopWatching<BombPlantSound>();
    }

    ON_CHANGE(FootstepSoundVisualizationEnabled)
    {
        if (newValue == true)
            hookContext.soundWatcher().template startWatching<FootstepSound>();
        else
            hookContext.soundWatcher().template stopWatching<FootstepSound>();
    }

    ON_CHANGE(WeaponReloadSoundVisualizationEnabled)
    {
        if (newValue == true)
            hookContext.soundWatcher().template startWatching<WeaponReloadSound>();
        else
            hookContext.soundWatcher().template stopWatching<WeaponReloadSound>();
    }

    ON_CHANGE(WeaponScopeSoundVisualizationEnabled)
    {
        if (newValue == true)
            hookContext.soundWatcher().template startWatching<WeaponScopeSound>();
        else
            hookContext.soundWatcher().template stopWatching<WeaponScopeSound>();
    }

    ON_CHANGE(model_glow_vars::Enabled)
    {
        if (newValue == false)
            modelGlowDeactivationFlags().set(ModelGlowDeactivationFlags::ModelGlowDeactivating);
    }

    ON_CHANGE(model_glow_vars::GlowDefuseKits)
    {
        if (newValue == false)
            modelGlowDeactivationFlags().set(ModelGlowDeactivationFlags::DefuseKitModelGlowDeactivating);
    }

    ON_CHANGE(model_glow_vars::GlowDroppedBomb)
    {
        if (newValue == false)
            modelGlowDeactivationFlags().set(ModelGlowDeactivationFlags::DroppedBombModelGlowDeactivating);
    }

    ON_CHANGE(model_glow_vars::GlowGrenadeProjectiles)
    {
        if (newValue == false)
            modelGlowDeactivationFlags().set(ModelGlowDeactivationFlags::GrenadeProjectileModelGlowDeactivating);
    }

    ON_CHANGE(model_glow_vars::GlowPlayers)
    {
        if (newValue == false)
            modelGlowDeactivationFlags().set(ModelGlowDeactivationFlags::PlayerModelGlowDeactivating);
    }

    ON_CHANGE(model_glow_vars::GlowTickingBomb)
    {
        if (newValue == false)
            modelGlowDeactivationFlags().set(ModelGlowDeactivationFlags::TickingBombModelGlowDeactivating);
    }

    ON_CHANGE(model_glow_vars::GlowWeapons)
    {
        if (newValue == false)
            modelGlowDeactivationFlags().set(ModelGlowDeactivationFlags::WeaponModelGlowDeactivating);
    }

    ON_CHANGE(outline_glow_vars::Enabled)
    {
        if (newValue == false)
            hookContext.template make<GlowSceneObjects>().clearObjects();
    }

    ON_CHANGE(player_info_vars::BombDefuseIconEnabled)
    {
        if (newValue)
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<DefuseIconPanel>();
        else
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<DefuseIconPanel>();
    }

    ON_CHANGE(player_info_vars::HostagePickupIconEnabled)
    {
        if (newValue)
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<HostagePickupPanel>();
        else
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<HostagePickupPanel>();
    }

    ON_CHANGE(player_info_vars::HostageRescueIconEnabled)
    {
        if (newValue)
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<HostageRescuePanel>();
        else
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<HostageRescuePanel>();
    }

    ON_CHANGE(player_info_vars::BlindedIconEnabled)
    {
        if (newValue)
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template set<BlindedIconPanel>();
        else
            hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow.template unset<BlindedIconPanel>();
    }

    ON_CHANGE(viewmodel_mod_vars::Enabled)
    {
        if (newValue == true && GET_CONFIG_VAR(viewmodel_mod_vars::ModifyFov))
            hookContext.template make<ClientModeHooks>().hookGetViewmodelFov();
        else
            hookContext.template make<ClientModeHooks>().restoreGetViewmodelFov();
    }

    ON_CHANGE(viewmodel_mod_vars::ModifyFov)
    {
        if (newValue == true && GET_CONFIG_VAR(viewmodel_mod_vars::Enabled))
            hookContext.template make<ClientModeHooks>().hookGetViewmodelFov();
        else
            hookContext.template make<ClientModeHooks>().restoreGetViewmodelFov();
    }

    ON_CHANGE(no_scope_inaccuracy_vis_vars::Enabled)
    {
        if (newValue == false)
            hookContext.template make<NoScopeInaccuracyVis>().onDisable();
    }

    ON_CHANGE(BombPlantAlertEnabled)
    {
        if (newValue == false)
            hookContext.template make<BombPlantAlert>().onDisable();
    }

    #undef ON_CHANGE

    [[nodiscard]] auto& modelGlowDeactivationFlags() const
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState.deactivationFlags;
    }

    HookContext& hookContext;
};
