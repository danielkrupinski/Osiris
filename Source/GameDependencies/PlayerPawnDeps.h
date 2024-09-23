#pragma once

#include <GameClasses/OffsetTypes/PlayerPawnOffset.h>

struct PlayerPawnDeps {
    PlayerPawnDeps() = default;

    template <typename PlayerPawnPatterns>
    explicit PlayerPawnDeps(const PlayerPawnPatterns& playerPawnPatterns) noexcept
        : offsetToPlayerPawnImmunity{playerPawnPatterns.offsetToPlayerPawnImmunity()}
        , offsetToWeaponServices{playerPawnPatterns.offsetToWeaponServices()}
        , offsetToPlayerController{playerPawnPatterns.offsetToPlayerController()}
        , offsetToIsDefusing{playerPawnPatterns.offsetToIsDefusing()}
        , offsetToIsPickingUpHostage{playerPawnPatterns.offsetToIsPickingUpHostage()}
        , offsetToHostageServices{playerPawnPatterns.offsetToHostageServices()}
        , offsetToFlashBangEndTime{playerPawnPatterns.offsetToFlashBangEndTime()}
    {
    }

    OffsetToPlayerPawnImmunity offsetToPlayerPawnImmunity;
    OffsetToWeaponServices offsetToWeaponServices;
    OffsetToPlayerController offsetToPlayerController;
    OffsetToIsDefusing offsetToIsDefusing;
    OffsetToIsPickingUpHostage offsetToIsPickingUpHostage;
    OffsetToHostageServices offsetToHostageServices;
    OffsetToFlashBangEndTime offsetToFlashBangEndTime;
};
