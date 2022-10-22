#pragma once

#include <cstdint>
#include <type_traits>

namespace inventory_changer
{

enum class TournamentMap : std::uint8_t {
    Ancient = 1,
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

constexpr auto operator<=>(TournamentMap a, TournamentMap b) noexcept
{
    return static_cast<std::underlying_type_t<TournamentMap>>(a) <=> static_cast<std::underlying_type_t<TournamentMap>>(b);
}

}
