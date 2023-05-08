#pragma once

#include <cassert>
#include <memory>
#include <optional>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/ItemGenerator/ItemGenerator.h>

#include "InventoryHandler.h"
#include "ItemRemovalHandler.h"

// TODO: remove this dependency
#include "../../../Memory.h"

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class ItemActivationHandler {
public:
    ItemActivationHandler(const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, ItemGenerator itemGenerator, InventoryHandler inventoryHandler, ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler, ResponseAccumulator responseAccumulator)
        : gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup }, itemGenerator{ itemGenerator }, inventoryHandler{ inventoryHandler }, itemRemovalHandler{ itemRemovalHandler }, responseAccumulator{ responseAccumulator } {}

    void activateOperationPass(ItemIterator operationPass) const
    {
        const auto& gameItem = operationPass->gameItem();
        assert(gameItem.isOperationPass());

        const auto coinID = gameItem.getWeaponID() != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1) : WeaponId::BronzeOperationHydraCoin;
        if (const auto operationCoin = gameItemLookup.findItem(coinID)) {
            inventoryHandler.addItem(inventory::Item{ *operationCoin }, true);
            itemRemovalHandler(operationPass);
        }
    }

    void activateViewerPass(ItemIterator viewerPass) const
    {
        const auto& gameItem = viewerPass->gameItem();
        assert(gameItem.isViewerPass());

        const auto coinID = Helpers::bronzeEventCoinFromViewerPass(gameItem.getWeaponID());
        if (coinID == WeaponId{})
            return;

        if (const auto eventCoin = gameItemLookup.findItem(coinID)) {
            const auto addedEventCoin = inventoryHandler.addItem(inventory::Item{ *eventCoin, inventory::TournamentCoin{ gameItemLookup.getStorage().hasExtraSouvenirTokens(gameItem) ? 3u : 0u } }, true);
            itemRemovalHandler(viewerPass);
            responseAccumulator(response::ViewerPassActivated{ addedEventCoin });
        }
    }

    void openContainer(ItemIterator container, ItemIterator key) const
    {
        assert(container->gameItem().isCrate() && key->gameItem().isCaseKey());

        auto generatedItem = itemGenerator.generateItemFromContainer(*container, std::to_address(key));
        if (!generatedItem.has_value())
            return;

        itemRemovalHandler(key);
        itemRemovalHandler(container);
        const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
        responseAccumulator(response::ContainerOpened{ receivedItem });
    }

    void openKeylessContainer(ItemIterator container) const
    {
        assert(container->gameItem().isCrate());

        auto generatedItem = itemGenerator.generateItemFromContainer(*container, nullptr);
        if (!generatedItem.has_value())
            return;

        itemRemovalHandler(container);
        const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
        responseAccumulator(response::ContainerOpened{ receivedItem });
    }

private:
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
    ItemGenerator itemGenerator;
    InventoryHandler inventoryHandler;
    ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler;
    ResponseAccumulator responseAccumulator;
};

}
