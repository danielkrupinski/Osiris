#pragma once

#include <MemoryPatterns/PlayerPawnPatterns.h>

struct PlayerPawnImpl {
    explicit PlayerPawnImpl(const PlayerPawnPatterns& playerPawnPatterns) noexcept
        : offsetToPlayerPawnImmunity{playerPawnPatterns.offsetToPlayerPawnImmunity()}
        , offsetToWeaponServices{playerPawnPatterns.offsetToWeaponServices()}
        , offsetToPlayerController{playerPawnPatterns.offsetToPlayerController()}
        , offsetToIsDefusing{playerPawnPatterns.offsetToIsDefusing()}
    {
    }

    OffsetToPlayerPawnImmunity offsetToPlayerPawnImmunity;
    OffsetToWeaponServices offsetToWeaponServices;
    OffsetToPlayerController offsetToPlayerController;
    OffsetToIsDefusing offsetToIsDefusing;
};
