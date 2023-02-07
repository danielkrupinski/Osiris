#pragma once

#include <cstdint>

#include <CSGO/Constants/ItemId.h>

#include "../../Memory.h"
#include <InventoryChanger/EconItemFunctions.h>
#include <InventoryChanger/EconItemViewFunctions.h>
#include <Interfaces/OtherInterfaces.h>
#include <RetSpoof/FunctionInvoker.h>
#include <CSGO/Functions.h>
#include <MemorySearch/BytePatternLiteral.h>

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
    Inventory(OtherInterfaces interfaces, const Memory& memory, const PatternFinder& clientPatternFinder)
        : interfaces{ interfaces }, memory{ memory }, econItemFunctions{ createEconItemFunctions(clientPatternFinder) }, econItemViewFunctions{ createEconItemViewFunctions(clientPatternFinder) },
#if IS_WIN32()
        createEconItemSharedObject{ retSpoofGadgets->client, clientPatternFinder("55 8B EC 83 EC 1C 8D 45 E4 C7 45"_pat).add(20).deref().get() },
        uiComponentInventory{ clientPatternFinder("C6 44 24 ? ? 83 3D"_pat).add(7).deref().as<csgo::UiComponentInventoryPOD**>() },
        setItemSessionPropertyValue{ clientPatternFinder("E8 ? ? ? ? 8B 4C 24 2C 46"_pat).add(1).relativeToAbsolute().as<csgo::SetItemSessionPropertyValue>() }
#elif IS_LINUX()
        createEconItemSharedObject{ retSpoofGadgets->client, clientPatternFinder("55 48 8D 05 ? ? ? ? 31 D2 BF"_pat).add(47).relativeToAbsolute().get() },
        uiComponentInventory{ clientPatternFinder("4C 89 3D ? ? ? ? 4C 89 FF EB 9D 0F 1F 44 00 00"_pat).add(3).relativeToAbsolute().as<csgo::UiComponentInventoryPOD**>() },
        setItemSessionPropertyValue{ clientPatternFinder("E8 ? ? ? ? 48 8B 85 ? ? ? ? 41 83 C4 01"_pat).add(1).relativeToAbsolute().as<csgo::SetItemSessionPropertyValue>() }
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
