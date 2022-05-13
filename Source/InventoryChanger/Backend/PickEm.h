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
        groups[group][indexInGroup] = team;
    }

    [[nodiscard]] TournamentTeam getPickedTeam(std::uint16_t group, std::uint8_t indexInGroup) const
    {
        if (const auto groupIt = groups.find(group); groupIt != groups.end()) {
            if (const auto pickIt = groupIt->second.find(indexInGroup); pickIt != groupIt->second.end())
                return pickIt->second;
        }
        return TournamentTeam::None;
    }

private:
    using PicksInGroup = std::unordered_map<std::uint8_t, TournamentTeam>;
    std::unordered_map<std::uint16_t, PicksInGroup> groups;
};

}
