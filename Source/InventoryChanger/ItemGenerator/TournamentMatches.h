#pragma once

#include <span>
#include <variant>

#include <SDK/ItemSchema.h>

namespace inventory_changer
{
    enum class TournamentMap : std::uint8_t;
}

namespace inventory_changer::item_generator
{

struct MatchWithMVPs {
    TournamentMap map;
    TournamentStage stage;
    TournamentTeam team1;
    TournamentTeam team2;
    std::array<csgo::ProPlayer, 10> mvpPlayers;

    [[nodiscard]] bool hasMVPs() const noexcept { return std::ranges::find(mvpPlayers, csgo::ProPlayer{}) != mvpPlayers.begin(); }
    [[nodiscard]] csgo::ProPlayer getRandomMVP() const noexcept
    {
        if (!hasMVPs())
            return csgo::ProPlayer{};
        return mvpPlayers[Helpers::random(static_cast<std::size_t>(0), static_cast<std::size_t>(std::distance(mvpPlayers.begin(), std::ranges::find(mvpPlayers, csgo::ProPlayer{}))) - 1)];
    }
};

struct Match {
    TournamentMap map;
    TournamentStage stage;
    TournamentTeam team1;
    TournamentTeam team2;
};

[[nodiscard]] std::variant<std::span<const MatchWithMVPs>, std::span<const Match>> getTournamentMatchesOnMap(std::uint32_t tournamentID, TournamentMap map) noexcept;

}
