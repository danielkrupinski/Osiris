#pragma once

#include <optional>
#include <utility>

#include <CS2/Classes/Entities/C_CSGO_PreviewPlayer.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/Entities/TeamNumber.h>
#include <Utils/Optional.h>

#include "BaseEntityForModelGlowPreview.h"
#include "PlayerControllerForModelGlowPreview.h"

template <typename HookContext>
class PlayerPawnForModelGlowPreview {
public:
    using RawType = cs2::C_CSGO_PreviewPlayer;

    PlayerPawnForModelGlowPreview(HookContext& hookContext, cs2::C_CSPlayerPawn* playerPawnPointer, TeamNumber playerTeamNumber) noexcept
        : hookContext{hookContext}
        , playerPawnPointer{playerPawnPointer}
        , playerTeamNumber{playerTeamNumber}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntityForModelGlowPreview>(playerPawnPointer);
    }

    [[nodiscard]] decltype(auto) playerController() const noexcept
    {
        return hookContext.template make<PlayerControllerForModelGlowPreview>();
    }

    [[nodiscard]] TeamNumber teamNumber() const noexcept
    {
        return playerTeamNumber;
    }

    [[nodiscard]] std::optional<bool> isAlive() const noexcept
    {
        return true;
    }

    [[nodiscard]] Optional<bool> hasImmunity() const noexcept
    {
        return false;
    }

    [[nodiscard]] Optional<int> health() const noexcept
    {
        return hookContext.playerModelGlowPreviewState().previewPlayerHealth;
    }

    [[nodiscard]] bool isControlledByLocalPlayer() const noexcept
    {
        return false;
    }

    [[nodiscard]] bool isTTorCT() const noexcept
    {
        return true;
    }

    [[nodiscard]] std::optional<bool> isEnemy() const noexcept
    {
        return true;
    }

    template <typename... Args>
    void setSceneObjectUpdater(Args&&... args) const noexcept
    {
        playerPawn().setSceneObjectUpdater(std::forward<Args>(args)...);
    }

    [[nodiscard]] decltype(auto) getSceneObjectUpdater() const noexcept
    {
        return playerPawn().getSceneObjectUpdater();
    }

private:
    [[nodiscard]] decltype(auto) playerPawn() const noexcept
    {
        return hookContext.template make<PlayerPawn>(playerPawnPointer);
    }

    HookContext& hookContext;
    cs2::C_CSPlayerPawn* playerPawnPointer;
    TeamNumber playerTeamNumber;
};
