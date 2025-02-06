#pragma once

#include <utility>

#include "PlayerStateIconsPanelContext.h"
#include "PlayerStateIconsToShow.h"

template <typename HookContext, typename Context = PlayerStateIconsPanelContext<HookContext>>
class PlayerStateIconsPanel {
public:
    template <typename... Args>
    explicit PlayerStateIconsPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.state().playerStateIconsToShow) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);

        auto&& playerStateChildren = context.panel().children();
        playerStateChildren[0].setVisible(context.state().playerStateIconsToShow.template has<DefuseIconPanel>() && playerPawn.isDefusing().valueOr(false));
        playerStateChildren[1].setVisible(context.state().playerStateIconsToShow.template has<HostagePickupPanel>() && playerPawn.isPickingUpHostage().valueOr(false));
        playerStateChildren[2].setVisible(context.state().playerStateIconsToShow.template has<HostageRescuePanel>() && playerPawn.isRescuingHostage());
        updateBlindedIconPanel(playerStateChildren[3], playerPawn);
    }

private:
    void updateBlindedIconPanel(auto&& blindedIconPanel, auto&& playerPawn) const noexcept
    {
        if (!context.state().playerStateIconsToShow.template has<BlindedIconPanel>()) {
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

    Context context;
};
