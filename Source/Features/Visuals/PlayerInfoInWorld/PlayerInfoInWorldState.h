#pragma once

#include "PlayerStateIcons/PlayerStateIconsToShow.h"

struct PlayerInfoInWorldState {
    PlayerStateIconsToShow playerStateIconsToShow{
        PlayerStateIconsToShow{}
            .set<DefuseIconPanel>()
            .set<HostagePickupPanel>()
            .set<HostageRescuePanel>()
            .set<BlindedIconPanel>()};
};
