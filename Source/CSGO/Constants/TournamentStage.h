#pragma once

#include <cstdint>

namespace csgo
{

enum class TournamentStage : std::uint8_t {
    GroupStage = 2,
    Quarterfinal = 5,
    Semifinal = 8,
    GrandFinal = 11,
    AllStar = 14,
    ChallengersStage = 27
};

}
