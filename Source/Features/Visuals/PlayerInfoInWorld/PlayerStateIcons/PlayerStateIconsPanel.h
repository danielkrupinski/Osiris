#pragma once

#include <utility>

#include "PlayerStateIconsToShow.h"

template <typename HookContext>
class PlayerStateIconsPanel {
public:
    explicit PlayerStateIconsPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel, PlayerInfoPanelCacheEntry&) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!iconsToShow()) {
            panel().setVisible(false);
            return;
        }

        panel().setVisible(true);

        auto&& playerStateChildren = panel().children();
        playerStateChildren[0].setVisible(iconsToShow().template has<DefuseIconPanel>() && playerPawn.isDefusing().valueOr(false));
        playerStateChildren[1].setVisible(iconsToShow().template has<HostagePickupPanel>() && playerPawn.isPickingUpHostage().valueOr(false));
        playerStateChildren[2].setVisible(iconsToShow().template has<HostageRescuePanel>() && playerPawn.isRescuingHostage());
        updateBlindedIconPanel(playerStateChildren[3], playerPawn);
    }

private:
    void updateBlindedIconPanel(auto&& blindedIconPanel, auto&& playerPawn) const noexcept
    {
        if (!iconsToShow().template has<BlindedIconPanel>()) {
            blindedIconPanel.setVisible(false);
            return;
        }

        const auto remainingFlashBangTime = playerPawn.getRemainingFlashBangTime();
        constexpr auto kFullBlindEnd{3.0f};
        constexpr auto kBlindEnd{1.0f};
        constexpr auto kPartiallyBlindDuration{kFullBlindEnd - kBlindEnd};
        if (remainingFlashBangTime <= kBlindEnd) {
            blindedIconPanel.setVisible(false);
            return;
        }

        blindedIconPanel.setVisible(true);
        const auto opacity = remainingFlashBangTime >= kFullBlindEnd ? 1.0f : (remainingFlashBangTime - kBlindEnd) / kPartiallyBlindDuration;
        blindedIconPanel.setOpacity(opacity);
    }

    [[nodiscard]] auto& iconsToShow() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState.playerStateIconsToShow;
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
};
