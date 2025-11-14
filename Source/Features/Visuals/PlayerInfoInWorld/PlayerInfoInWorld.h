#pragma once

#include <Features/Common/InWorldPanels.h>
#include <Features/Common/FeatureToggle.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <Hooks/ViewRenderHook.h>

#include "PlayerInfoInWorldContext.h"
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
        auto&& condition = context().condition();
        if (!condition.shouldRun() || !condition.shouldDrawInfoOnPawn(playerPawn))
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
    [[nodiscard]] decltype(auto) context() const noexcept
    {
        return hookContext.template make<PlayerInfoInWorldContext>();
    }

    HookContext& hookContext;
};
