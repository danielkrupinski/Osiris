#pragma once

#include <cstdint>
#include <map>

#include <CSGO/Constants/Tournament.h>
#include <CSGO/Constants/TournamentTeam.h>

namespace inventory_changer::backend
{

class PickEm {
public:
    struct PickPosition {
        csgo::Tournament tournament;
        std::uint16_t group;
        std::uint8_t indexInGroup;

        friend auto operator<=>(const PickPosition&, const PickPosition&) = default;
    };

    void pick(PickPosition position, csgo::TournamentTeam team)
    {
        picks[position] = team;
    }

    [[nodiscard]] csgo::TournamentTeam getPickedTeam(PickPosition position) const
    {
        if (const auto pick = picks.find(position); pick != picks.end())
            return pick->second;
        return csgo::TournamentTeam::None;
    }

    [[nodiscard]] const auto& getPicks() const noexcept
    {
        return picks;
    }

    void clear() noexcept
    {
        picks.clear();
    }

private:
    std::map<PickPosition, csgo::TournamentTeam> picks;
};

}
