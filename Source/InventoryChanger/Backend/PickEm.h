#pragma once

#include <cstdint>
#include <map>

#include <SDK/ItemSchema.h>

namespace inventory_changer::backend
{

class PickEm {
public:
    struct PickPosition {
        std::uint8_t tournament;
        std::uint16_t group;
        std::uint8_t indexInGroup;

        friend auto operator<=>(const PickPosition&, const PickPosition&) = default;
    };

    void pick(std::uint16_t group, std::uint8_t indexInGroup, TournamentTeam team)
    {
        PickPosition position;
        position.tournament = 19;
        position.group = group;
        position.indexInGroup = indexInGroup;

        picks[position] = team;
    }

    [[nodiscard]] TournamentTeam getPickedTeam(std::uint16_t group, std::uint8_t indexInGroup) const
    {
        PickPosition position;
        position.tournament = 19;
        position.group = group;
        position.indexInGroup = indexInGroup;
        
        if (const auto pick = picks.find(position); pick != picks.end())
            return pick->second;
        return TournamentTeam::None;
    }

private:
    std::map<PickPosition, TournamentTeam> picks;
};

}
