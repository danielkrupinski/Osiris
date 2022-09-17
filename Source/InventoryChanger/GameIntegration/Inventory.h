#pragma once

#include <cstdint>

#include <SDK/Constants/ItemId.h>

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
};

}
