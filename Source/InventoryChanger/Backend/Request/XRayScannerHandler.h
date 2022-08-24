#pragma once

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/ItemGenerator/ItemGenerator.h>

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class XRayScannerHandler {
public:
    XRayScannerHandler(const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, XRayScanner& xRayScanner, InventoryHandler<ResponseAccumulator> inventoryHandler, ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler, ResponseAccumulator responseAccumulator, ItemConstRemover constRemover)
        : gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup }, xRayScanner{ xRayScanner }, inventoryHandler{ inventoryHandler }, itemRemovalHandler{ itemRemovalHandler }, responseAccumulator{responseAccumulator}, constRemover{constRemover} {}

    void performXRayScan(ItemIterator crate) const
    {
        if (!crate->gameItem().isCrate())
            return;

        auto generatedItem = item_generator::generateItemFromContainer(gameItemLookup, crateLootLookup, *crate, nullptr);
        if (!generatedItem.has_value())
            return;

        constRemover(crate).setState(inventory::Item::State::InXrayScanner);
        responseAccumulator(response::ItemHidden{ crate });

        generatedItem->setState(inventory::Item::State::InXrayScanner);

        const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
        xRayScanner.storeItems(XRayScanner::Items{ receivedItem, crate });
        responseAccumulator(response::XRayScannerUsed{ receivedItem });
    }

    void claimXRayScannedItem(ItemIterator crate, std::optional<ItemIterator> key) const
    {
        const auto scannerItems = xRayScanner.getItems();
        if (!scannerItems.has_value())
            return;

        if (crate != scannerItems->crate)
            return;

        if (key.has_value()) {
            if (const auto& keyItem = *key; keyItem->gameItem().isCaseKey()) {
                constRemover(scannerItems->reward).getProperties().common.tradableAfterDate = keyItem->getProperties().common.tradableAfterDate;
                itemRemovalHandler.removeItem(keyItem);
            }
        }

        itemRemovalHandler.removeItem(crate);
        constRemover(scannerItems->reward).setState(inventory::Item::State::Default);
        responseAccumulator(response::ItemUnhidden{ scannerItems->reward });
        responseAccumulator(response::XRayItemClaimed{ scannerItems->reward });
    }

private:
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
    XRayScanner& xRayScanner;
    InventoryHandler<ResponseAccumulator> inventoryHandler;
    ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler;
    ResponseAccumulator responseAccumulator;
    ItemConstRemover constRemover;
};

}
