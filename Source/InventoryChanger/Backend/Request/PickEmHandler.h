#pragma once

#include <InventoryChanger/Backend/PickEm.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>

#include <SDK/Constants/TournamentTeam.h>

namespace inventory_changer::backend
{

class PickEmHandler {
public:
    PickEmHandler(PickEm& pickEm, ResponseQueue<>& responseQueue)
        : pickEm{ pickEm }, responseQueue{ responseQueue } {}

    void pickSticker(PickEm::PickPosition position, csgo::TournamentTeam team) const
    {
        pickEm.pick(position, team);
        responseQueue.add(response::PickEmUpdated{});
    }

    void clearPicks() const
    {
        pickEm.clear();
        responseQueue.add(response::PickEmUpdated{});
    }

private:
    PickEm& pickEm;
    ResponseQueue<>& responseQueue;
};

}
