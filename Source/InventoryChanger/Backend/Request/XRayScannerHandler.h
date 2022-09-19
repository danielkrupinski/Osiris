#pragma once

#include <cassert>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/ItemGenerator/ItemGenerator.h>

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class XRayScannerHandler {
public:
    XRayScannerHandler(const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, XRayScanner& xRayScanner, InventoryHandler inventoryHandler, ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler, ResponseAccumulator responseAccumulator, ItemConstRemover constRemover)
        : gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup }, xRayScanner{ xRayScanner }, inventoryHandler{ inventoryHandler }, itemRemovalHandler{ itemRemovalHandler }, responseAccumulator{responseAccumulator}, constRemover{constRemover} {}

    void performXRayScan(ItemIterator crate) const
    {
        assert(crate->gameItem().isCrate());

        Helpers::RandomGenerator randomGenerator;
        auto generatedItem = item_generator::generateItemFromContainer(randomGenerator, gameItemLookup, crateLootLookup, *crate, nullptr);
        if (!generatedItem.has_value())
            return;

        constRemover(crate).setState(inventory::Item::State::InXrayScanner);
        responseAccumulator(response::ItemHidden{ crate });

        generatedItem->setState(inventory::Item::State::InXrayScanner);

        const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
        xRayScanner.storeItems(XRayScanner::Items{ receivedItem, crate });
        responseAccumulator(response::XRayScannerUsed{ receivedItem });
    }

    void claimXRayScannedItem(ItemIterator crate, ItemIterator key) const
    {
        assert(crate->gameItem().isCrate() && key->gameItem().isCaseKey());

        const auto scannerItems = xRayScanner.getItems();
        if (!scannerItems.has_value())
            return;

        if (crate != scannerItems->crate)
            return;

        constRemover(scannerItems->reward).getProperties().common.tradableAfterDate = key->getProperties().common.tradableAfterDate;
        responseAccumulator(response::TradabilityUpdated{ scannerItems->reward });
        itemRemovalHandler(key);
        itemRemovalHandler(crate);
        constRemover(scannerItems->reward).setState(inventory::Item::State::Default);
        responseAccumulator(response::XRayItemClaimed{ scannerItems->reward });
    }

    void claimXRayScannedItemWithoutKey(ItemIterator crate) const
    {
        assert(crate->gameItem().isCrate());

        const auto scannerItems = xRayScanner.getItems();
        if (!scannerItems.has_value())
            return;

        if (crate != scannerItems->crate)
            return;

        itemRemovalHandler(crate);
        constRemover(scannerItems->reward).setState(inventory::Item::State::Default);
        responseAccumulator(response::XRayItemClaimed{ scannerItems->reward });
    }

private:
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
    XRayScanner& xRayScanner;
    InventoryHandler inventoryHandler;
    ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler;
    ResponseAccumulator responseAccumulator;
    ItemConstRemover constRemover;
};

}
