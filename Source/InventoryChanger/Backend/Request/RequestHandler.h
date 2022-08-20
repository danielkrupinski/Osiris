#pragma once

#include "RequestTypes.h"
#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Response/Response.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>
#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/GameItems/Lookup.h>

namespace inventory_changer::backend
{

class ItemIDMap;
class Loadout;
class PickEm;
class StorageUnitManager;
class XRayScanner;

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

class LoadoutHandler {
public:
    LoadoutHandler(Loadout& loadout, ResponseQueue<>& responseQueue)
        : loadout{ loadout }, responseQueue{ responseQueue } {}

    void equipItem(ItemIterator item, Loadout::Slot slot, Team team) const
    {
        switch (team) {
        case Team::None:
            loadout.equipItemNoTeam(item, slot);
            break;
        case Team::TT:
            loadout.equipItemTT(item, slot);
            break;
        case Team::CT:
            loadout.equipItemCT(item, slot);
            break;
        default:
            return;
        }

        responseQueue.add(response::ItemEquipped{ item, slot, team });
    }

    void markItemEquipped(ItemIterator item, Loadout::Slot slot, Team team) const
    {
        switch (team) {
        case Team::None:
            loadout.equipItemNoTeam(item, slot);
            break;
        case Team::TT:
            loadout.equipItemTT(item, slot);
            break;
        case Team::CT:
            loadout.equipItemCT(item, slot);
            break;
        default:
            break;
        }
    }

private:
    Loadout& loadout;
    ResponseQueue<>& responseQueue;
};

struct RequestHandler {
    RequestHandler(StorageUnitManager& storageUnitManager, XRayScanner& xRayScanner, ResponseQueue<>& responseQueue, ItemList& inventory, Loadout& loadout, ItemIDMap& itemIDMap, const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, ItemConstRemover constRemover)
        : storageUnitManager{ storageUnitManager }, xRayScanner{ xRayScanner }, responseQueue{ responseQueue }, inventory{ inventory }, loadout{ loadout }, itemIDMap{ itemIDMap }, gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup }, constRemover{ constRemover } {}

    void operator()(const request::ApplySticker& request) const;
    void operator()(const request::WearSticker& request) const;
    void operator()(const request::SwapStatTrak& request) const;
    void operator()(const request::OpenContainer& request) const;
    void operator()(const request::ApplyPatch& request) const;
    void operator()(const request::RemovePatch& request) const;
    void operator()(const request::ActivateOperationPass& request) const;
    void operator()(const request::ActivateViewerPass& request) const;
    void operator()(const request::AddNameTag& request) const;
    void operator()(const request::RemoveNameTag& request) const;
    void operator()(const request::ActivateSouvenirToken& request) const;
    void operator()(const request::UnsealGraffiti& request) const;
    void operator()(const request::UpdateStatTrak& request) const;
    void operator()(const request::SelectTeamGraffiti& request) const;
    void operator()(const request::MarkItemUpdated& request) const;
    void operator()(const request::PerformXRayScan& request) const;
    void operator()(const request::ClaimXRayScannedItem& request) const;
    void operator()(const request::NameStorageUnit& request) const;
    void operator()(const request::MarkStorageUnitModified& request) const;
    void operator()(const request::BindItemToStorageUnit& request) const;
    void operator()(const request::AddItemToStorageUnit& request) const;
    void operator()(const request::RemoveFromStorageUnit& request) const;
    void operator()(const request::UpdateStorageUnitAttributes& request) const;
    void operator()(const request::MoveItemToFront& request) const;
    ItemIterator operator()(request::AddItem request) const;
    ItemIterator operator()(const request::RemoveItem& request) const;

private:
    ItemIterator removeItemInternal(ItemIterator it) const;

    StorageUnitManager& storageUnitManager;
    XRayScanner& xRayScanner;
    ResponseQueue<>& responseQueue;
    ItemList& inventory;
    Loadout& loadout;
    ItemIDMap& itemIDMap;
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
    ItemConstRemover constRemover;
};

}
