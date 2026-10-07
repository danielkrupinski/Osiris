#pragma once

#include <utility>

#include <Common/Visibility.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>

template <typename HookContext>
class BombStatusPanel {
public:
    explicit BombStatusPanel(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void hide()
    {
        changeVisibility(Visibility::Hidden, [this] { bombStatusPanel().setParent(invisiblePanel()); });
    }

    void restore()
    {
        changeVisibility(Visibility::Visible, [this] { bombStatusPanel().setParent(scoreAndTimeAndBombPanel()); });
    }

    void onUnload()
    {
        restore();
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state().invisiblePanelHandle);
    }

private:
    void changeVisibility(Visibility targetVisibility, auto performVisibilityChange)
    {
        if (auto& visibility = state().visibility; visibility != targetVisibility) {
            performVisibilityChange();
            visibility = targetVisibility;
        }
    }
    
    [[nodiscard]] decltype(auto) bombStatusPanel() const
    {
        return hookContext.hud().bombStatus();
    }

    [[nodiscard]] decltype(auto) scoreAndTimeAndBombPanel() const
    {
        return hookContext.hud().scoreAndTimeAndBomb();
    }

    [[nodiscard]] decltype(auto) invisiblePanel() const
    {
        return  hookContext.template make<PanelHandle>(state().invisiblePanelHandle).getOrInit(createInvisiblePanel());
    }

    [[nodiscard]] decltype(auto) state() const
    {
        return hookContext.bombStatusPanelState();
    }

    [[nodiscard]] decltype(auto) createInvisiblePanel() const noexcept
    {
        return [this] () -> decltype(auto) {
            auto&& invisiblePanel = hookContext.panelFactory().createPanel(scoreAndTimeAndBombPanel()).uiPanel();
            invisiblePanel.setVisible(false);
            return utils::lvalue<decltype(invisiblePanel)>(invisiblePanel);
        };
    }
    
    HookContext& hookContext;
};
