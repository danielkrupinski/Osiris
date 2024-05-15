#pragma once

#include <MemoryPatterns/PlayerPawnPatterns.h>

struct PlayerPawnImpl {
    explicit PlayerPawnImpl(const PlayerPawnPatterns& playerPawnPatterns) noexcept
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
