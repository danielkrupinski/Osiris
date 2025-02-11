#pragma once

#include <GameClient/Panorama/PanelHandle.h>

template <typename HookContext>
struct BombStatusPanelContext {
    explicit BombStatusPanelContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) bombStatusPanel() const noexcept
    {
        return hookContext.hud().bombStatus();
    }

    [[nodiscard]] decltype(auto) scoreAndTimeAndBombPanel() const noexcept
    {
        return hookContext.hud().scoreAndTimeAndBomb();
    }

    [[nodiscard]] decltype(auto) invisiblePanel() const noexcept
    {
        return invisiblePanelHandle().getOrInit(createInvisiblePanel());
    }

    [[nodiscard]] auto& visibility() const noexcept
    {
        return state().visibility;
    }

private:
    [[nodiscard]] decltype(auto) state() const noexcept
    {
        return hookContext.bombStatusPanelState();
    }

    [[nodiscard]] decltype(auto) invisiblePanelHandle() const noexcept
    {
        return hookContext.template make<PanelHandle>(state().invisiblePanelHandle);
    }

    [[nodiscard]] decltype(auto) createInvisiblePanel() const noexcept
    {
        return [this] {
            auto&& invisiblePanel = hookContext.panelFactory().createPanel(scoreAndTimeAndBombPanel()).uiPanel();
            invisiblePanel.setVisible(false);
            return invisiblePanel;
        };
    }

    HookContext& hookContext;
};
