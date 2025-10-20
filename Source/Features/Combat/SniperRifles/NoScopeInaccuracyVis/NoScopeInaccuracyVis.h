#pragma once

#include <GameClient/Crosshair.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <GameClient/WorldToScreen/ViewToProjectionMatrix.h>
#include "NoScopeInaccuracyVisParams.h"
#include "NoScopeInaccuracyVisConfigVariables.h"

template <typename HookContext>
class NoScopeInaccuracyVis {
public:
    NoScopeInaccuracyVis(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update() const
    {
        if (!enabled())
            return;

        const auto visible = shouldShow();
        auto&& panel = getPanel();
        panel.setVisible(visible);
        if (visible) {
            panel.setHeight(computeHeightFromInaccuracy());
            using namespace no_scope_inaccuracy_vis_params;
            const auto color = hookContext.template make<Crosshair>().getColor().valueOr(kFallbackColor);
            panel.setBorder(kBorderWidth, color.setAlpha(kBorderAlpha));
            panel.setBackgroundColor(color.setAlpha(kBackgroundAlpha));
        }
    }

    void onDisable() const
    {
        hookContext.template make<PanelHandle>(state().panelHandle).get().hide();
    }

    void onUnload() const
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state().panelHandle);
    }

private:
    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(no_scope_inaccuracy_vis_vars::Enabled);
    }

    [[nodiscard]] bool shouldShow() const
    {
        auto&& localPlayerPawn = hookContext.activeLocalPlayerPawn();
        return localPlayerPawn
            && !localPlayerPawn.isScoped().valueOr(true)
            && localPlayerPawn.isUsingSniperRifle();
    }

    [[nodiscard]] cs2::CUILength computeHeightFromInaccuracy() const
    {
        const auto y = hookContext.template make<ViewToProjectionMatrix>().transformY(getInaccuracy());
        return cs2::CUILength::percent(y.value_or(0.0f) * 100.0f);
    }

    [[nodiscard]] float getInaccuracy() const
    {
        return hookContext.localPlayerBulletInaccuracy().valueOr(0.0f);
    }

    [[nodiscard]] decltype(auto) getPanel() const
    {
        return hookContext.template make<PanelHandle>(state().panelHandle).getOrInit(createPanel());
    }

    [[nodiscard]] auto createPanel() const noexcept
    {
        return [this] () -> decltype(auto) {
            auto&& panel = hookContext.panelFactory().createPanel(hookContext.hud().getHudReticle()).uiPanel();
            panel.setWidth(cs2::CUILength::heightPercentage(100));
            panel.setBorderRadius(cs2::CUILength::percent(50));
            panel.setAlign(PanelAlignmentParams{
                .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
                .verticalAlignment = cs2::k_EVerticalAlignmentCenter});
            return utils::lvalue<decltype(panel)>(panel);
        };
    }

    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().noScopeInaccuracyVisState;
    }
    
    HookContext& hookContext;
};
