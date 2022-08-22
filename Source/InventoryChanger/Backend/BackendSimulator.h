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
#include "Request/LoadoutHandler.h"
#include "Request/PickEmHandler.h"
#include "Request/RequestHandler.h"
#include "Response/Response.h"
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
        return RequestHandler{ storageUnitManager, xRayScanner, responseQueue, inventory, loadout, itemIDMap, gameItemLookup, crateLootLookup, ItemConstRemover{ inventory } };
    }

    [[nodiscard]] PickEmHandler getPickEmHandler()
    {
        return PickEmHandler{ pickEm, responseQueue };
    }

    [[nodiscard]] LoadoutHandler getLoadoutHandler()
    {
        return LoadoutHandler{ loadout, responseQueue };
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
    const auto requestHandler = backend.getRequestHandler();

    for (auto it = inventory.cbegin(); it != inventory.cend();)
        it = requestHandler(request::RemoveItem{ it });
}

}
