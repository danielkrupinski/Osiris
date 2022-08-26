#pragma once

#include <cassert>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Response/ResponseTypes.h>
#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class ItemModificationHandler {
public:
    ItemModificationHandler(ItemConstRemover constRemover, ResponseAccumulator responseAccumulator)
        : constRemover{ constRemover }, responseAccumulator{ responseAccumulator } {}

    void updateStatTrak(ItemIterator item, int newStatTrak) const
    {
        const auto statTrak = inventory::getStatTrak(constRemover(item));
        if (!statTrak)
            return;

        *statTrak = newStatTrak;
        responseAccumulator(response::StatTrakUpdated{ item, newStatTrak });
    }

    void selectTeamGraffiti(ItemIterator tournamentCoin, std::uint16_t graffitiID) const
    {
        assert(tournamentCoin->gameItem().isTournamentCoin());
        responseAccumulator(response::TeamGraffitiSelected{ tournamentCoin, graffitiID });
    }

private:
    ItemConstRemover constRemover;
    ResponseAccumulator responseAccumulator;
};
}
