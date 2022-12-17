#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <span>
#include <variant>

#include <CSGO/Constants/ProPlayer.h>
#include <CSGO/Constants/TournamentTeam.h>
#include <CSGO/ItemSchema.h>

namespace csgo
{
    enum class Tournament : std::uint8_t;
    enum class TournamentStage : std::uint8_t;
}

namespace inventory_changer
{
    enum class TournamentMap : std::uint8_t;
}

namespace inventory_changer::item_generator
{

struct MatchWithMVPs {
    TournamentMap map;
    csgo::TournamentStage stage;
    csgo::TournamentTeam team1;
    csgo::TournamentTeam team2;
    std::array<csgo::ProPlayer, 10> mvpPlayers;
};

[[nodiscard]] constexpr std::size_t countMVPs(const MatchWithMVPs& matchWithMVPs)
{
    return static_cast<std::size_t>(std::distance(matchWithMVPs.mvpPlayers.begin(), std::ranges::find(matchWithMVPs.mvpPlayers, csgo::ProPlayer{})));
}

struct Match {
    TournamentMap map;
    csgo::TournamentStage stage;
    csgo::TournamentTeam team1;
    csgo::TournamentTeam team2;
};

[[nodiscard]] std::variant<std::span<const MatchWithMVPs>, std::span<const Match>> getTournamentMatchesOnMap(csgo::Tournament tournament, TournamentMap map) noexcept;

}
