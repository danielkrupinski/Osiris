#pragma once

#include <cstdint>
#include <unordered_map>

#include <SDK/ItemSchema.h>

namespace inventory_changer::backend
{

class PickEm {
public:
    void pick(std::uint16_t group, std::uint8_t indexInGroup, TournamentTeam team)
    {
        tournaments[19][group][indexInGroup] = team;
    }

    [[nodiscard]] TournamentTeam getPickedTeam(std::uint16_t group, std::uint8_t indexInGroup) const
    {
        if (const auto tournamentIt = tournaments.find(19); tournamentIt != tournaments.end()) {
            if (const auto groupIt = tournamentIt->second.find(group); groupIt != tournamentIt->second.end()) {
                if (const auto pickIt = groupIt->second.find(indexInGroup); pickIt != groupIt->second.end())
                    return pickIt->second;
            }
        }
        return TournamentTeam::None;
    }

private:
    using PicksInGroup = std::unordered_map<std::uint8_t, TournamentTeam>;
    using GroupsInTournament = std::unordered_map<std::uint16_t, PicksInGroup>;
    std::unordered_map<std::uint8_t, GroupsInTournament> tournaments;
};

}
