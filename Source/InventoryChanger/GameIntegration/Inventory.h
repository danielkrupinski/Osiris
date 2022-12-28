#pragma once

#include <cstdint>

#include <CSGO/Constants/ItemId.h>

#include "../../Memory.h"
#include <InventoryChanger/EconItemFunctions.h>
#include <InventoryChanger/EconItemViewFunctions.h>
#include <Interfaces/OtherInterfaces.h>
#include <RetSpoof/FunctionInvoker.h>
#include <CSGO/Functions.h>

namespace csgo
{
    enum class StickerId : int;
    enum class Team;
}

namespace inventory_changer
{
namespace game_items
{
    class Storage;
}
namespace inventory
{
    class Item;
}
enum class ItemId : csgo::ItemId;
}

enum class Team;

namespace inventory_changer::game_integration
{

class Inventory {
public:
    Inventory(OtherInterfaces interfaces, const Memory& memory, const helpers::PatternFinder& clientPatternFinder)
        : interfaces{ interfaces }, memory{ memory }, econItemFunctions{ createEconItemFunctions(clientPatternFinder) }, econItemViewFunctions{ createEconItemViewFunctions(clientPatternFinder) },
#if IS_WIN32()
        createEconItemSharedObject{ retSpoofGadgets->client, clientPatternFinder("\x55\x8B\xEC\x83\xEC\x1C\x8D\x45\xE4\xC7\x45").add(20).deref().get() },
        uiComponentInventory{ clientPatternFinder("\xC6\x44\x24??\x83\x3D").add(7).deref().as<csgo::UiComponentInventoryPOD**>() },
        setItemSessionPropertyValue{ clientPatternFinder("\xE8????\x8B\x4C\x24\x2C\x46").add(1).relativeToAbsolute().as<csgo::SetItemSessionPropertyValue>() }
#elif IS_LINUX()
        createEconItemSharedObject{ retSpoofGadgets->client, clientPatternFinder("\x55\x48\x8D\x05????\x31\xD2\x4C\x8D\x0D").add(50).relativeToAbsolute().get() },
        uiComponentInventory{ clientPatternFinder("\xE8????\x4C\x89\x3D????\x4C\x89\xFF\xEB\x9E").add(8).relativeToAbsolute().as<csgo::UiComponentInventoryPOD**>() },
        setItemSessionPropertyValue{ clientPatternFinder("\xE8????\x48\x8B\x85????\x41\x83\xC4\x01").add(1).relativeToAbsolute().as<csgo::SetItemSessionPropertyValue>() }
#endif
    {
    }

    ItemId createSOCItem(const game_items::Storage& gameItemStorage, const inventory::Item& inventoryItem, bool asUnacknowledged);
    [[nodiscard]] ItemId assingNewItemID(ItemId itemID);
    void applySticker(ItemId itemID, csgo::StickerId stickerID, std::uint8_t slot);
    void removeSticker(ItemId itemID, std::uint8_t slot);
    void updateStickerWear(ItemId itemID, std::uint8_t slot, float newWear);
    void viewerPassActivated(ItemId tournamentCoinItemID);
    void addNameTag(ItemId itemID, const char* newNameTag);
    void removeNameTag(ItemId itemID);
    void deleteItem(ItemId itemID);
    void updateStatTrak(ItemId itemID, int newStatTrakValue);
    void containerOpened(ItemId unlockedItemID);
    void applyPatch(ItemId itemID, int patchID, std::uint8_t slot);
    void removePatch(ItemId itemID, std::uint8_t slot);
    void souvenirTokenActivated(ItemId tournamentCoinItemID, std::uint32_t dropsAwarded);
    void unsealGraffiti(ItemId itemID);
    void selectTeamGraffiti(ItemId itemID, std::uint16_t graffitiID);
    void statTrakSwapped(ItemId itemID);
    void equipItem(ItemId itemID, csgo::Team team, std::uint8_t slot);
    void markItemUpdated(ItemId itemID);
    void pickEmUpdated();
    void hideItem(ItemId itemID);
    void unhideItem(ItemId itemID);
    void xRayItemRevealed(ItemId itemID);
    void xRayItemClaimed(ItemId itemID);
    void nameStorageUnit(ItemId itemID, const char* newName);
    void storageUnitModified(ItemId itemID, std::uint32_t modificationDate, std::uint32_t itemCount);
    void addItemToStorageUnit(ItemId itemID, ItemId storageUnitItemID);
    void itemAddedToStorageUnit(ItemId storageUnitItemID);
    void removeItemFromStorageUnit(ItemId itemID, ItemId storageUnitItemID);
    void updateTradableAfterDate(ItemId itemID, std::uint32_t tradableAfterDate);

    [[nodiscard]] const EconItemViewFunctions& getEconItemViewFunctions() const noexcept
    {
        return econItemViewFunctions;
    }

private:
    OtherInterfaces interfaces;
    const Memory& memory;
    EconItemFunctions econItemFunctions;
    EconItemViewFunctions econItemViewFunctions;
    FunctionInvoker<csgo::CreateSharedObjectSubclass<csgo::EconItemPOD>> createEconItemSharedObject;
    csgo::UiComponentInventoryPOD** uiComponentInventory;
    csgo::SetItemSessionPropertyValue setItemSessionPropertyValue;
};

}
