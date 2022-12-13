#pragma once

#include <chrono>
#include <queue>
#include <variant>

#include "Item.h"
#include "Items.h"
#include "ItemIDMap.h"
#include "Loadout.h"
#include "PickEm.h"
#include "StorageUnitManager.h"
#include "XRayScanner.h"
#include "Request/ItemActivationHandler.h"
#include "Request/ItemModificationHandler.h"
#include "Request/LoadoutHandler.h"
#include "Request/PickEmHandler.h"
#include "Request/RequestHandler.h"
#include "Request/StorageUnitHandler.h"
#include "Request/XRayScannerHandler.h"
#include "Response/Response.h"
#include "Response/ResponseAccumulator.h"
#include "Response/ResponseHandler.h"
#include "Response/ResponseQueue.h"

#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/GameItems/Lookup.h>

namespace inventory_changer::backend
{

class BackendSimulator {
public:
    explicit BackendSimulator(game_items::Lookup gameItemLookup, game_items::CrateLootLookup crateLootLookup, const Memory& memory, Helpers::RandomGenerator& randomGenerator)
        : gameItemLookup{ std::move(gameItemLookup) }, crateLootLookup{ std::move(crateLootLookup) }, memory{ memory }, randomGenerator{ randomGenerator } {}

    [[nodiscard]] const Loadout& getLoadout() const noexcept
    {
        return items.loadout;
    }

    [[nodiscard]] const PickEm& getPickEm() const noexcept
    {
        return pickEm;
    }

    [[nodiscard]] const ItemList& getInventory() const noexcept
    {
        return items.inventory;
    }

    [[nodiscard]] const game_items::Lookup& getGameItemLookup() const noexcept
    {
        return gameItemLookup;
    }

    [[nodiscard]] const game_items::CrateLootLookup& getCrateLootLookup() const noexcept
    {
        return crateLootLookup;
    }

    [[nodiscard]] const ItemIDMap& getItemIDMap() const noexcept
    {
        return items.itemIDMap;
    }

    [[nodiscard]] const StorageUnitManager& getStorageUnitManager() const noexcept
    {
        return items.storageUnitManager;
    }

    [[nodiscard]] const XRayScanner& getXRayScanner() const noexcept
    {
        return items.xRayScanner;
    }

    [[nodiscard]] std::optional<ItemIterator> itemFromID(ItemId itemID) const
    {
        return items.itemIDMap.get(itemID);
    }

    [[nodiscard]] std::optional<ItemId> getItemID(ItemIterator it) const
    {
        return items.itemIDMap.getItemID(it);
    }

    [[nodiscard]] RequestHandler getRequestHandler()
    {
        return RequestHandler{ getItemModificationHandler(), getItemRemovalHandler(), getInventoryHandler(), getStorageUnitHandler(), getResponseAccumulator(), items.inventory, gameItemLookup, ItemConstRemover{ items.inventory } };
    }

    [[nodiscard]] PickEmHandler<ResponseAccumulator> getPickEmHandler()
    {
        return PickEmHandler{ pickEm, getResponseAccumulator() };
    }

    [[nodiscard]] LoadoutHandler<ResponseAccumulator> getLoadoutHandler()
    {
        return LoadoutHandler{ items.loadout, getResponseAccumulator() };
    }

    [[nodiscard]] InventoryHandler getInventoryHandler()
    {
        return InventoryHandler{ items.inventory, getResponseAccumulator() };
    }

    [[nodiscard]] ItemRemovalHandler<ResponseAccumulator> getItemRemovalHandler()
    {
        return ItemRemovalHandler<ResponseAccumulator>{ items };
    }

    [[nodiscard]] StorageUnitHandler<ResponseAccumulator> getStorageUnitHandler()
    {
        return StorageUnitHandler{ items.storageUnitManager, ItemConstRemover{ items.inventory }, getInventoryHandler(), getResponseAccumulator() };
    }

    [[nodiscard]] XRayScannerHandler<ResponseAccumulator> getXRayScannerHandler()
    {
        return XRayScannerHandler{ getItemGenerator(), items.xRayScanner, getInventoryHandler(), getItemRemovalHandler(), getResponseAccumulator(), ItemConstRemover{ items.inventory } };
    }

    [[nodiscard]] ItemModificationHandler<ResponseAccumulator> getItemModificationHandler()
    {
        return ItemModificationHandler{ ItemConstRemover{ items.inventory }, getResponseAccumulator() };
    }

    [[nodiscard]] ItemActivationHandler<ResponseAccumulator> getItemActivationHandler()
    {
        return ItemActivationHandler{ gameItemLookup, crateLootLookup, getItemGenerator(), getInventoryHandler(), getItemRemovalHandler(), getResponseAccumulator() };
    }

    template <typename GameInventory>
    void run(GameInventory& gameInventory, std::chrono::milliseconds delay)
    {
        items.responseQueue.visit(ResponseHandler{ gameItemLookup.getStorage(), items.itemIDMap, items.storageUnitManager, gameInventory }, delay);
    }

    [[nodiscard]] bool isInXRayScan() const noexcept
    {
        return items.xRayScanner.getItems().has_value();
    }

    [[nodiscard]] ItemGenerator getItemGenerator() const noexcept
    {
        return ItemGenerator{ memory, randomGenerator, gameItemLookup, crateLootLookup };
    }

private:
    [[nodiscard]] ResponseAccumulator getResponseAccumulator()
    {
        return ResponseAccumulator{ items.responseQueue };
    }

    game_items::Lookup gameItemLookup;
    game_items::CrateLootLookup crateLootLookup;
    const Memory& memory;
    Helpers::RandomGenerator& randomGenerator;
    Items items;
    PickEm pickEm;
};

inline void clearInventory(BackendSimulator& backend)
{
    const auto& inventory = backend.getInventory();
    const auto itemRemovalHandler = backend.getItemRemovalHandler();

    for (auto it = inventory.cbegin(); it != inventory.cend();)
        it = itemRemovalHandler(it);
}

}
