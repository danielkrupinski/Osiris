#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <GameClasses/PanoramaImagePanel.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <Utils/Lvalue.h>

#include "BombSiteIconPanel.h"
#include "BombTimerCondition.h"
#include "BombTimerPanel.h"
#include "BombTimerPanelFactory.h"
#include "BombTimerState.h"
#include "BombTimerTextPanel.h"

template <typename HookContext>
struct BombTimerContext {
    BombTimerContext(HookContext& context) noexcept
        : _context{context}
    {
    }

    [[nodiscard]] bool hasTickingC4() const noexcept
    {
        const auto plantedC4{_context.plantedC4()};
        return plantedC4 && plantedC4->isTicking().valueOr(true) && plantedC4->getTimeToExplosion().greaterThan(0.0f).valueOr(false);
    }

    [[nodiscard]] auto tickingC4() const noexcept
    {
        return *_context.plantedC4();
    }

    [[nodiscard]] auto bombTimerCondition() const noexcept
    {
        return BombTimerCondition{*this};
    }

    [[nodiscard]] auto bombTimerPanel() const noexcept
    {
        return BombTimerPanel{*this};
    }

    [[nodiscard]] decltype(auto) bombPlantedPanel() const noexcept
    {
        return _context.hud().bombPlantedPanel();
    }

    [[nodiscard]] decltype(auto) bombTimerContainerPanel() const noexcept
    {
        if (auto&& bombTimerContainer = uiEngine().getPanelFromHandle(state().bombTimerContainerPanelHandle))
            return utils::lvalue<decltype(bombTimerContainer)>(bombTimerContainer);

        updatePanelHandles();
        return uiEngine().getPanelFromHandle(state().bombTimerContainerPanelHandle);
    }

    [[nodiscard]] auto bombSiteIconPanel() const noexcept
    {
        return BombSiteIconPanel{uiEngine().getPanelFromHandle(state().bombSiteIconPanelHandle).clientPanel().template as<PanoramaImagePanel>()};
    }

    [[nodiscard]] auto bombTimerTextPanel() const noexcept
    {
        return BombTimerTextPanel{uiEngine().getPanelFromHandle(state().bombTimerPanelHandle).clientPanel().template as<PanoramaLabel>()};
    }

    void updatePanelHandles() const noexcept
    {
        auto&& factory = _context.template make<BombTimerPanelFactory>();
        auto&& containerPanel = factory.createContainerPanel(_context.hud().scoreAndTimeAndBomb());
        state().bombTimerContainerPanelHandle = containerPanel.getHandle();

        auto&& bombSiteIconPanel = factory.createBombSiteIconPanel(containerPanel);
        state().bombSiteIconPanelHandle = bombSiteIconPanel.getHandle();

        auto&& timerTextPanel = factory.createTimerTextPanel(containerPanel);
        state().bombTimerPanelHandle = timerTextPanel.getHandle();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return _context.featuresStates().hudFeaturesStates.bombTimerState;
    }

private:
    [[nodiscard]] decltype(auto) uiEngine() const noexcept
    {
        return _context.template make<PanoramaUiEngine>();
    }

    HookContext& _context;
};
