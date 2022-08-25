#pragma once

#include <chrono>
#include <queue>
#include <variant>

#include "Item.h"
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
    explicit BackendSimulator(const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup)
        : gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup } {}

    [[nodiscard]] const Loadout& getLoadout() const noexcept
    {
        return loadout;
    }

    [[nodiscard]] const PickEm& getPickEm() const noexcept
    {
        return pickEm;
    }

    [[nodiscard]] const ItemList& getInventory() const noexcept
    {
        return inventory;
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
        return itemIDMap;
    }

    [[nodiscard]] const StorageUnitManager& getStorageUnitManager() const noexcept
    {
        return storageUnitManager;
    }

    [[nodiscard]] const XRayScanner& getXRayScanner() const noexcept
    {
        return xRayScanner;
    }

    [[nodiscard]] std::optional<ItemIterator> itemFromID(std::uint64_t itemID) const
    {
        return itemIDMap.get(itemID);
    }

    [[nodiscard]] std::optional<std::uint64_t> getItemID(ItemIterator it) const
    {
        return itemIDMap.getItemID(it);
    }

    [[nodiscard]] RequestHandler getRequestHandler()
    {
        return RequestHandler{ getItemModificationHandler(), getItemRemovalHandler(), getInventoryHandler(), getStorageUnitHandler(), xRayScanner, ResponseAccumulator{ responseQueue }, inventory, loadout, gameItemLookup, crateLootLookup, ItemConstRemover{ inventory } };
    }

    [[nodiscard]] PickEmHandler<ResponseAccumulator> getPickEmHandler()
    {
        return PickEmHandler{ pickEm, ResponseAccumulator{ responseQueue } };
    }

    [[nodiscard]] LoadoutHandler<ResponseAccumulator> getLoadoutHandler()
    {
        return LoadoutHandler{ loadout, ResponseAccumulator{ responseQueue } };
    }

    [[nodiscard]] InventoryHandler<ResponseAccumulator> getInventoryHandler()
    {
        return InventoryHandler{ inventory, ResponseAccumulator{ responseQueue } };
    }

    [[nodiscard]] ItemRemovalHandler<ResponseAccumulator> getItemRemovalHandler()
    {
        return ItemRemovalHandler{ storageUnitManager, xRayScanner, responseQueue, inventory, loadout, itemIDMap, getStorageUnitHandler() };
    }

    [[nodiscard]] StorageUnitHandler<ResponseAccumulator> getStorageUnitHandler()
    {
        return StorageUnitHandler{ storageUnitManager, ItemConstRemover{ inventory }, getInventoryHandler(), ResponseAccumulator{ responseQueue } };
    }

    [[nodiscard]] XRayScannerHandler<ResponseAccumulator> getXRayScannerHandler()
    {
        return XRayScannerHandler{ gameItemLookup, crateLootLookup, xRayScanner, getInventoryHandler(), getItemRemovalHandler(), ResponseAccumulator{ responseQueue }, ItemConstRemover{ inventory } };
    }

    [[nodiscard]] ItemModificationHandler<ResponseAccumulator> getItemModificationHandler()
    {
        return ItemModificationHandler{ ItemConstRemover{ inventory }, ResponseAccumulator{ responseQueue } };
    }

    [[nodiscard]] ItemActivationHandler<ResponseAccumulator> getItemActivationHandler()
    {
        return ItemActivationHandler{ gameItemLookup, crateLootLookup, getInventoryHandler(), getItemRemovalHandler(), ResponseAccumulator{ responseQueue } };
    }

    template <typename GameInventory>
    void run(GameInventory& gameInventory, std::chrono::milliseconds delay)
    {
        responseQueue.visit(ResponseHandler{ gameItemLookup.getStorage(), itemIDMap, storageUnitManager, gameInventory }, delay);
    }

    [[nodiscard]] bool isInXRayScan() const noexcept
    {
        return xRayScanner.getItems().has_value();
    }

private:
    ItemList inventory;
    Loadout loadout;
    ResponseQueue<> responseQueue;
    ItemIDMap itemIDMap;
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
    PickEm pickEm;
    XRayScanner xRayScanner;
    StorageUnitManager storageUnitManager;
};

inline void clearInventory(BackendSimulator& backend)
{
    const auto& inventory = backend.getInventory();
    const auto itemRemovalHandler = backend.getItemRemovalHandler();

    for (auto it = inventory.cbegin(); it != inventory.cend();)
        it = itemRemovalHandler.removeItem(it);
}

}
