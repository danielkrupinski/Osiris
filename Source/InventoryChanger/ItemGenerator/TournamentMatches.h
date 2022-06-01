#pragma once

#include <span>
#include <variant>

#include <SDK/ItemSchema.h>

namespace inventory_changer::game_items
{
    enum class TournamentMap : std::uint8_t;
}

namespace inventory_changer::item_generator
{

struct Match {
    game_items::TournamentMap map;
    TournamentStage stage;
    TournamentTeam team1;
    TournamentTeam team2;
    std::array<ProPlayer, 10> mvpPlayers;

    [[nodiscard]] bool hasMVPs() const noexcept { return std::ranges::find(mvpPlayers, ProPlayer{}) != mvpPlayers.begin(); }
    [[nodiscard]] ProPlayer getRandomMVP() const noexcept
    {
        if (!hasMVPs())
            return ProPlayer{};
        return mvpPlayers[Helpers::random(static_cast<std::size_t>(0), static_cast<std::size_t>(std::distance(mvpPlayers.begin(), std::ranges::find(mvpPlayers, ProPlayer{}))) - 1)];
    }
};

struct MatchWithoutMVPs {
    game_items::TournamentMap map;
    TournamentStage stage;
    TournamentTeam team1;
    TournamentTeam team2;
};

[[nodiscard]] std::variant<std::span<const Match>, std::span<const MatchWithoutMVPs>> getTournamentMatchesOnMap(std::uint32_t tournamentID, game_items::TournamentMap map) noexcept;

}
