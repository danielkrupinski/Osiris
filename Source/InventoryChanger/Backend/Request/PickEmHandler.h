#pragma once

#include <InventoryChanger/Backend/PickEm.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>

#include <CSGO/Constants/TournamentTeam.h>

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class PickEmHandler {
public:
    PickEmHandler(PickEm& pickEm, ResponseAccumulator responseAccumulator)
        : pickEm{ pickEm }, responseAccumulator{ responseAccumulator } {}

    void pickSticker(PickEm::PickPosition position, csgo::TournamentTeam team) const
    {
        pickEm.pick(position, team);
        responseAccumulator(response::PickEmUpdated{});
    }

    void clearPicks() const
    {
        pickEm.clear();
        responseAccumulator(response::PickEmUpdated{});
    }

private:
    PickEm& pickEm;
    ResponseAccumulator responseAccumulator;
};

}
