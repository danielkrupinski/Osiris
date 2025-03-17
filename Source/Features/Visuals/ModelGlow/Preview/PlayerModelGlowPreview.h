#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <GameClient/Entities/TeamNumber.h>
#include <Features/Visuals/ModelGlow/ModelGlow.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <Features/Visuals/ModelGlow/PlayerModelGlow/PlayerModelGlow.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreviewColorMode.h>
#include <GameClient/EntitySystem/EntitySystem.h>

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

        state().shouldUpdatePanel = shouldUpdatePreviewPanel(colorMode, previewPlayerColorIndex, previewPlayerHealth);
        state().previewPlayerHealth = previewPlayerHealth;
        state().colorMode = colorMode;
        state().previewPlayerColorIndex = previewPlayerColorIndex;
    }

    void hookPreviewPlayersSceneObjectUpdaters() const noexcept
    {
        hookContext.template make<ModelGlow>().updateSceneObjectUpdaterHook(previewPlayerTT());
        hookContext.template make<ModelGlow>().updateSceneObjectUpdaterHook(previewPlayerCT());
    }

    void onUnload() const noexcept
    {
        hookContext.template make<PlayerModelGlow>().onUnload(previewPlayerTT());
        hookContext.template make<PlayerModelGlow>().onUnload(previewPlayerCT());
    }

    void applyPreviewPlayerModelGlow(auto&& previewPlayer) const noexcept
    {
        hookContext.template make<ModelGlow>().applyPlayerModelGlow(hookContext.template make<PlayerPawnForModelGlowPreview>(previewPlayer, previewPlayerTeamNumber(previewPlayer)));
    }

private:
    void updateAnimationProgress() const noexcept
    {
        constexpr auto kDefaultFrameTime{1 / 60.0f};
        const auto frameTime = hookContext.globalVars().frametime().valueOr(kDefaultFrameTime);

        using namespace player_model_glow_preview_params;
        if (frameTime < kAnimationCycleDuration - state().animationProgress)
            state().animationProgress += frameTime;
        else
            state().animationProgress = 0.0f;
    }

    [[nodiscard]] auto computePreviewPlayerHealth() const noexcept
    {
        return healthAnimationStepToHealthValue(healthAnimationStep());
    }

    [[nodiscard]] auto computePreviewPlayerColorIndex() const noexcept
    {
        const auto step = colorIndexAnimationStep();
        return isPlayerColorStep(step) ? playerColorIndexFromStep(step) : -1;
    }

    [[nodiscard]] auto computeColorMode() const noexcept
    {
        if (!hookContext.config().template getVariable<ModelGlowEnabled>() || !hookContext.config().template getVariable<PlayerModelGlowEnabled>())
            return PlayerModelGlowPreviewColorMode::None;

        switch (hookContext.config().template getVariable<PlayerModelGlowColorMode>()) {
        case PlayerModelGlowColorType::HealthBased: return PlayerModelGlowPreviewColorMode::HealthBased;
        case PlayerModelGlowColorType::PlayerOrTeamColor: return PlayerModelGlowPreviewColorMode::PlayerOrTeamColor;
        case PlayerModelGlowColorType::TeamColor: return PlayerModelGlowPreviewColorMode::TeamColor;
        default: return PlayerModelGlowPreviewColorMode::None;
        }
    }

    [[nodiscard]] bool shouldUpdatePreviewPanel(auto colorMode, auto previewPlayerColorIndex, auto previewPlayerHealth) const noexcept
    {
        if (colorMode != state().colorMode)
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

    [[nodiscard]] static cs2::CCSPlayerController::m_iCompTeammateColor playerColorIndexFromStep(int step) noexcept
    {
        using namespace player_model_glow_preview_params::player_color_animation_params;
        return step - kTeamColorSteps;
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

    HookContext& hookContext;
};
