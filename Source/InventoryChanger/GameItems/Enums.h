#pragma once

#include <cstdint>

namespace game_items
{

enum class TournamentMap : std::uint8_t {
    None = 0,
    Ancient,
    Cache,
    Cobblestone,
    Dust2,
    Inferno,
    Mirage,
    Nuke,
    Overpass,
    Train,
    Vertigo
};

}
