#pragma once

#include <memory>
#include <optional>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/ItemGenerator/ItemGenerator.h>

#include "InventoryHandler.h"
#include "ItemRemovalHandler.h"

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class ItemActivationHandler {
public:
    ItemActivationHandler(const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, InventoryHandler<ResponseAccumulator> inventoryHandler, ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler, ResponseAccumulator responseAccumulator)
        : gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup }, inventoryHandler{ inventoryHandler }, itemRemovalHandler{ itemRemovalHandler }, responseAccumulator{ responseAccumulator } {}

    void activateOperationPass(ItemIterator operationPass) const
    {
        const auto& gameItem = operationPass->gameItem();
        if (!gameItem.isOperationPass())
            return;

        const auto coinID = gameItem.getWeaponID() != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1) : WeaponId::BronzeOperationHydraCoin;
        if (const auto operationCoin = gameItemLookup.findItem(coinID)) {
            inventoryHandler.addItem(inventory::Item{ *operationCoin }, true);
            itemRemovalHandler.removeItem(operationPass);
        }
    }

    void activateViewerPass(ItemIterator viewerPass) const
    {
        const auto& gameItem = viewerPass->gameItem();
        if (!gameItem.isViewerPass())
            return;

        const auto coinID = Helpers::bronzeEventCoinFromViewerPass(gameItem.getWeaponID());
        if (coinID == WeaponId::None)
            return;

        if (const auto eventCoin = gameItemLookup.findItem(coinID)) {
            const auto addedEventCoin = inventoryHandler.addItem(inventory::Item{ *eventCoin, inventory::TournamentCoin{ Helpers::numberOfTokensWithViewerPass(gameItem.getWeaponID()) }, }, true);
            itemRemovalHandler.removeItem(viewerPass);
            responseAccumulator(response::ViewerPassActivated{ addedEventCoin });
        }
    }

    void openContainer(ItemIterator container, ItemIterator key) const
    {
        if (!container->gameItem().isCrate())
            return;

        auto generatedItem = item_generator::generateItemFromContainer(gameItemLookup, crateLootLookup, *container, std::to_address(key));
        if (!generatedItem.has_value())
            return;

        if (key->gameItem().isCaseKey())
            itemRemovalHandler.removeItem(key);

        itemRemovalHandler.removeItem(container);
        const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
        responseAccumulator(response::ContainerOpened{ receivedItem });
    }

    void openKeylessContainer(ItemIterator container) const
    {
        if (!container->gameItem().isCrate())
            return;

        auto generatedItem = item_generator::generateItemFromContainer(gameItemLookup, crateLootLookup, *container, nullptr);
        if (!generatedItem.has_value())
            return;

        itemRemovalHandler.removeItem(container);
        const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
        responseAccumulator(response::ContainerOpened{ receivedItem });
    }

private:
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
    InventoryHandler<ResponseAccumulator> inventoryHandler;
    ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler;
    ResponseAccumulator responseAccumulator;
};

}
