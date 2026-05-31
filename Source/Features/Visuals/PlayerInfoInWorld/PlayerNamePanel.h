#pragma once

#include <algorithm>
#include <utility>

#include <CS2/Classes/Vector.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaTransformations.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <GameClient/WorldToScreen/ViewToProjectionMatrix.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>

#include "PlayerNamePanelParams.h"

template <typename HookContext>
class PlayerNamePanelContext {
public:
    PlayerNamePanelContext(HookContext& hookContext, cs2::CUIPanel* panel, auto&) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return _hookContext.config();
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

    [[nodiscard]] decltype(auto) toClipSpace(const cs2::Vector& worldPosition) const noexcept
    {
        return _hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(worldPosition);
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        return ViewToProjectionMatrix{_hookContext}.getFovScale();
    }

    [[nodiscard]] decltype(auto) panoramaTransformFactory() const noexcept
    {
        return _hookContext.panoramaTransformFactory();
    }

private:
    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};

template <typename HookContext, typename Context = PlayerNamePanelContext<HookContext>>
class PlayerNamePanel {
public:
    template <typename... Args>
    explicit PlayerNamePanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.config().template getVariable<player_info_vars::PlayerNameEnabled>()) {
            context.panel().setVisible(false);
            return;
        }

        const auto playerName = playerPawn.playerController().getName();
        if (!playerName || !*playerName) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);
        context.panel().children()[0].clientPanel().template as<PanoramaLabel>().setText(playerName);
    }

    void updatePosition(const cs2::Vector& origin) const noexcept
    {
        const auto positionInClipSpace = context.toClipSpace(origin);
        if (!positionInClipSpace.onScreen()) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setZIndex(-positionInClipSpace.z);

        constexpr auto kMaxScale{1.0f};
        const auto scale = std::clamp(500.0f / (positionInClipSpace.z / context.getFovScale() + 400.0f), 0.4f, kMaxScale);
        const auto deviceCoordinates = positionInClipSpace.toNormalizedDeviceCoordinates();

        PanoramaTransformations{
            context.panoramaTransformFactory().scale(scale),
            context.panoramaTransformFactory().translate(deviceCoordinates.getX(), deviceCoordinates.getY())
        }.applyTo(context.panel());
    }

private:
    Context context;
};
