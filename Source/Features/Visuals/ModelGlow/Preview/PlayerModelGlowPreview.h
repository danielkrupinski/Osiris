#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <GameClient/Entities/TeamNumber.h>
#include <Features/Visuals/ModelGlow/ModelGlow.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <Features/Visuals/ModelGlow/PlayerModelGlow/PlayerModelGlow.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreviewColorMode.h>
#include <GameClient/EntitySystem/EntitySystem.h>

#include "EnemyTeam.h"
#include "PlayerPawnForModelGlowPreview.h"
#include "PlayerModelGlowPreviewParams.h"

template <typename HookContext>
class PlayerModelGlowPreview {
public:
    explicit PlayerModelGlowPreview(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void setPreviewPlayerTT(auto&& previewPlayer) const noexcept
    {
        state().previewPlayerHandleTT = previewPlayer.baseEntity().handle();
    }

    [[nodiscard]] bool isPreviewPlayerSetTT() const noexcept
    {
        return static_cast<bool>(hookContext.template make<EntitySystem>().getEntityFromHandle(state().previewPlayerHandleTT));
    }

    void setPreviewPlayerCT(auto&& previewPlayer) const noexcept
    {
        state().previewPlayerHandleCT = previewPlayer.baseEntity().handle();
    }

    [[nodiscard]] bool isPreviewPlayerSetCT() const noexcept
    {
        return static_cast<bool>(hookContext.template make<EntitySystem>().getEntityFromHandle(state().previewPlayerHandleCT));
    }

    void update() const noexcept
    {
        updateAnimationProgress();

        const auto previewPlayerHealth = computePreviewPlayerHealth();
        const auto previewPlayerColorIndex = computePreviewPlayerColorIndex();
        const auto colorMode = computeColorMode();
        const auto enemyTeam = computeEnemyTeam();

        state().shouldUpdatePanel = shouldUpdatePreviewPanel(colorMode, previewPlayerColorIndex, previewPlayerHealth, enemyTeam);
        state().previewPlayerHealth = previewPlayerHealth;
        state().colorMode = colorMode;
        state().previewPlayerColorIndex = previewPlayerColorIndex;
        state().enemyTeam = enemyTeam;
    }

    void hookPreviewPlayersSceneObjectUpdaters() const noexcept
    {
        hookContext.template make<ModelGlow>().updateInMainThread()(PlayerModelGlow{hookContext}, previewPlayerTT(), EntityTypeInfo{});
        hookContext.template make<ModelGlow>().updateInMainThread()(PlayerModelGlow{hookContext}, previewPlayerCT(), EntityTypeInfo{});
    }

    void onUnload() const noexcept
    {
        hookContext.template make<ModelGlow>().onUnload()(PlayerModelGlow{hookContext}, previewPlayerTT());
        hookContext.template make<ModelGlow>().onUnload()(PlayerModelGlow{hookContext}, previewPlayerCT());
    }

    void applyPreviewPlayerModelGlow(auto&& previewPlayer) const noexcept
    {
        hookContext.template make<ModelGlow>().updateInSceneObjectUpdater()(PlayerModelGlow{hookContext}, hookContext.template make<PlayerPawnForModelGlowPreview>(previewPlayer, previewPlayerTeamNumber(previewPlayer)), EntityTypeInfo{});
    }

private:
    [[nodiscard]] bool previewActive() const noexcept
    {
        return getConfigVariable<model_glow_vars::Enabled>() && getConfigVariable<model_glow_vars::GlowPlayers>();
    }

    void updateAnimationProgress() const noexcept
    {
        if (!previewActive()) {
            state().animationProgress = 0.0f;
            return;
        }

        constexpr auto kDefaultFrameTime{1 / 60.0f};
        const auto frameTime = hookContext.globalVars().frametime().valueOr(kDefaultFrameTime);

        using namespace player_model_glow_preview_params;
        if (frameTime < kAnimationCycleDuration - state().animationProgress)
            state().animationProgress += frameTime;
        else
            state().animationProgress = 0.0f;
    }

    [[nodiscard]] auto computeEnemyTeam() const noexcept
    {
        if (getConfigVariable<model_glow_vars::GlowPlayers>() && getConfigVariable<model_glow_vars::GlowOnlyEnemies>())
            return EnemyTeam::Both;

        if (getConfigVariable<model_glow_vars::PlayerGlowColorMode>() == PlayerModelGlowColorType::EnemyAlly)
            return enemyTeamAnimationStep() == 0 ? EnemyTeam::T : EnemyTeam::CT;

        return EnemyTeam::None;
    }

    [[nodiscard]] int enemyTeamAnimationStep() const noexcept
    {
        using namespace player_model_glow_preview_params::team_animation_params;
        return static_cast<int>(state().animationProgress / kStepDuration) % kSteps;
    }

    [[nodiscard]] auto computePreviewPlayerHealth() const noexcept
    {
        return healthAnimationStepToHealthValue(healthAnimationStep());
    }

    [[nodiscard]] auto computePreviewPlayerColorIndex() const noexcept
    {
        const auto step = colorIndexAnimationStep();
        return isPlayerColorStep(step) ? playerColorIndexFromStep(step) : cs2::PlayerColorIndex{-1};
    }

    [[nodiscard]] auto computeColorMode() const noexcept
    {
        if (!previewActive())
            return PlayerModelGlowPreviewColorMode::None;

        switch (getConfigVariable<model_glow_vars::PlayerGlowColorMode>()) {
        case PlayerModelGlowColorType::HealthBased: return PlayerModelGlowPreviewColorMode::HealthBased;
        case PlayerModelGlowColorType::PlayerOrTeamColor: return PlayerModelGlowPreviewColorMode::PlayerOrTeamColor;
        case PlayerModelGlowColorType::TeamColor: return PlayerModelGlowPreviewColorMode::TeamColor;
        case PlayerModelGlowColorType::EnemyAlly: return PlayerModelGlowPreviewColorMode::EnemyAlly;
        default: return PlayerModelGlowPreviewColorMode::None;
        }
    }

    [[nodiscard]] bool shouldUpdatePreviewPanel(auto colorMode, auto previewPlayerColorIndex, auto previewPlayerHealth, auto enemyTeam) const noexcept
    {
        if (colorMode != state().colorMode)
            return true;
        if (enemyTeam != state().enemyTeam)
            return true;

        switch (colorMode) {
        case PlayerModelGlowPreviewColorMode::TeamColor:
        case PlayerModelGlowPreviewColorMode::PlayerOrTeamColor:
            return previewPlayerColorIndex != state().previewPlayerColorIndex;
        case PlayerModelGlowPreviewColorMode::HealthBased:
            return previewPlayerHealth != state().previewPlayerHealth;
        default:
            return true;
        }
    }

    [[nodiscard]] auto& modelGlowState() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] int colorIndexAnimationStep() const noexcept
    {
        using namespace player_model_glow_preview_params::player_color_animation_params;
        return static_cast<int>(state().animationProgress / kStepDuration) % kSteps;
    }

    [[nodiscard]] static bool isPlayerColorStep(int step) noexcept
    {
        using namespace player_model_glow_preview_params::player_color_animation_params;
        return step >= kTeamColorSteps;
    }

    [[nodiscard]] static cs2::PlayerColorIndex playerColorIndexFromStep(int step) noexcept
    {
        using namespace player_model_glow_preview_params::player_color_animation_params;
        return cs2::PlayerColorIndex{step - kTeamColorSteps};
    }

    [[nodiscard]] int healthAnimationStepToHealthValue(int step) const noexcept
    {
        using namespace player_model_glow_preview_params::player_health_animation_params;
        if (step > kMaxPlayerHealth)
            return (step - kMaxPlayerHealth) + kMinPlayerHealth;
        else
            return (kMaxPlayerHealth - step) + kMinPlayerHealth;
    }

    [[nodiscard]] int healthAnimationStep() const noexcept
    {
        using namespace player_model_glow_preview_params::player_health_animation_params;
        return static_cast<int>(state().animationProgress / kStepDuration) % kSteps;
    }

    [[nodiscard]] decltype(auto) previewPlayerTT() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(state().previewPlayerHandleTT).entity().template as<PlayerPawnForModelGlowPreview>(TeamNumber::TT);
    }

    [[nodiscard]] decltype(auto) previewPlayerCT() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityIdentityFromHandle(state().previewPlayerHandleCT).entity().template as<PlayerPawnForModelGlowPreview>(TeamNumber::CT);
    }

    [[nodiscard]] auto previewPlayerTeamNumber(auto&& previewPlayer) const noexcept
    {
        return previewPlayer.baseEntity().handle() == state().previewPlayerHandleTT ? TeamNumber::TT : TeamNumber::CT;
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.playerModelGlowPreviewState();
    }

    template <typename ConfigVariable>
    [[nodiscard]] decltype(auto) getConfigVariable() const noexcept
    {
        return hookContext.config().template getVariable<ConfigVariable>(); 
    }

    HookContext& hookContext;
};
