#pragma once

#include <Features/Common/InWorldPanels.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <Hooks/ViewRenderHook.h>

#include "PlayerInfoInWorldState.h"
#include "PlayerPositionArrow/PlayerPositionArrowColorType.h"
#include "PlayerStateIcons/PlayerStateIconsToShow.h"

template <typename HookContext>
class PlayerInfoInWorld {
public:
    PlayerInfoInWorld(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void drawPlayerInformation(auto&& playerPawn) noexcept
    {
        if (!enabled() || !shouldDrawInfoOnPawn(playerPawn))
            return;

        const auto absOrigin = playerPawn.absOrigin();
        if (!absOrigin.hasValue())
            return;

        const auto positionInClipSpace = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(absOrigin.value());
        if (!positionInClipSpace.onScreen())
            return;

        auto&& playerInformationPanel = hookContext.template make<InWorldPanels>().getNextPlayerInfoPanel();
        playerInformationPanel.drawPlayerInfo(playerPawn);
        playerInformationPanel.updatePosition(absOrigin.value());
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return GET_CONFIG_VAR(player_info_vars::Enabled);
    }

    [[nodiscard]] bool shouldDrawInfoOnPawn(auto&& playerPawn) const noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!GET_CONFIG_VAR(player_info_vars::OnlyEnemies) || playerPawn.isEnemy().value_or(true));
    }

    HookContext& hookContext;
};
