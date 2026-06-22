#pragma once

#include <array>
#include <cstddef>
#include <optional>

#include <GameClient/Entities/PlayerController.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/StringBuilder.h>

#include "SpectatorListConfigVariables.h"
#include "SpectatorListPanelFactory.h"
#include "SpectatorListPanelParams.h"
#include "SpectatorListState.h"

template <typename HookContext>
class SpectatorList {
public:
    explicit SpectatorList(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , enabled{GET_CONFIG_VAR(SpectatorListEnabled)}
        , watchedPawnHandle{enabled ? getWatchedPawnHandle() : std::nullopt}
    {
    }

    void addPotentialSpectator(auto&& playerController) noexcept
    {
        if (!enabled || !watchedPawnHandle || spectatorCount >= spectatorNames.size())
            return;
        if (playerController == hookContext.localPlayerController())
            return;
        if (!playerController.isConnected())
            return;
        if (playerController.isPawnAlive().value_or(true))
            return;
        if (playerController.observerTargetHandle() != watchedPawnHandle)
            return;

        if (const auto name = playerController.getName(); name && *name)
            spectatorNames[spectatorCount++] = name;
    }

    void render() const noexcept
    {
        if (!enabled || spectatorCount == 0) {
            hide();
            return;
        }

        auto&& container = getOrCreateContainerPanel();
        container.setVisible(true);
        container.setHeight(cs2::CUILength::pixels(
            spectator_list_panel_params::container_panel_params::kHeaderHeight
            + spectator_list_panel_params::container_panel_params::kNameRowHeight * static_cast<float>(spectatorCount)
            + spectator_list_panel_params::container_panel_params::kBottomPadding));

        updateHeaderPanel(container);
        updateNamePanels(container);
    }

    void onDisable() const noexcept
    {
        hide();
    }

    void onUnload() const noexcept
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state().containerPanelHandle);
    }

private:
    [[nodiscard]] std::optional<cs2::CEntityHandle> getWatchedPawnHandle() const noexcept
    {
        auto&& localPlayerController = hookContext.localPlayerController();
        if (!localPlayerController)
            return std::nullopt;

        if (const auto isAlive = localPlayerController.isPawnAlive(); isAlive && !*isAlive)
            return localPlayerController.observerTargetHandle();
        return localPlayerController.playerPawnHandle();
    }

    [[nodiscard]] decltype(auto) getOrCreateContainerPanel() const noexcept
    {
        return hookContext.template make<PanelHandle>(state().containerPanelHandle).getOrInit([this]() -> decltype(auto) {
            auto&& factory = hookContext.template make<SpectatorListPanelFactory>();
            auto&& container = factory.createContainerPanel(hookContext.hud().rootPanel());
            state().containerPanelHandle = container.getHandle();
            state().headerPanelHandle = factory.createHeaderPanel(container).getHandle();
            return utils::lvalue<decltype(container)>(container);
        });
    }

    void updateHeaderPanel(auto&& container) const noexcept
    {
        auto&& header = hookContext.template make<PanelHandle>(state().headerPanelHandle).getOrInit([this, &container]() -> decltype(auto) {
            return hookContext.template make<SpectatorListPanelFactory>().createHeaderPanel(container);
        });
        StringBuilderStorage<32> storage;
        header.clientPanel().template as<PanoramaLabel>().setText(
            storage.builder().put("SPECTATORS (", spectatorCount, ')').cstring());
    }

    void updateNamePanels(auto&& container) const noexcept
    {
        auto&& factory = hookContext.template make<SpectatorListPanelFactory>();
        std::size_t index = 0;
        for (; index < spectatorCount; ++index) {
            auto&& namePanel = hookContext.template make<PanelHandle>(state().namePanelHandles[index]).getOrInit([&factory, &container]() -> decltype(auto) {
                return factory.createNamePanel(container);
            });
            namePanel.setVisible(true);
            namePanel.clientPanel().template as<PanoramaLabel>().setText(spectatorNames[index]);
        }

        for (; index < state().namePanelHandles.size(); ++index)
            hookContext.template make<PanelHandle>(state().namePanelHandles[index]).get().setVisible(false);
    }

    void hide() const noexcept
    {
        hookContext.template make<PanelHandle>(state().containerPanelHandle).get().setVisible(false);
    }

    [[nodiscard]] SpectatorListState& state() const noexcept
    {
        return hookContext.featuresStates().hudFeaturesStates.spectatorListState;
    }

    HookContext& hookContext;
    bool enabled;
    std::optional<cs2::CEntityHandle> watchedPawnHandle;
    std::array<const char*, SpectatorListState::kMaxSpectators> spectatorNames{};
    std::size_t spectatorCount{};
};
