#pragma once

#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::game_integration
{

class Inventory {
public:
    std::uint64_t createSOCItem(const inventory::Item& inventoryItem, bool asUnacknowledged);
    [[nodiscard]] std::uint64_t assingNewItemID(std::uint64_t itemID);
    void applySticker(std::uint64_t itemID, int stickerID, std::uint8_t slot);
    void removeSticker(std::uint64_t itemID, std::uint8_t slot);
    void updateStickerWear(std::uint64_t itemID, std::uint8_t slot, float newWear);
    void viewerPassActivated(std::uint64_t tournamentCoinItemID);
    void addNameTag(std::uint64_t itemID, const char* newNameTag);
    void removeNameTag(std::uint64_t itemID);
    void deleteItem(std::uint64_t itemID);
    void updateStatTrak(std::uint64_t itemID, int newStatTrakValue);
    void containerOpened(std::uint64_t unlockedItemID);
    void applyPatch(std::uint64_t itemID, int patchID, std::uint8_t slot);
    void removePatch(std::uint64_t itemID, std::uint8_t slot);
    void souvenirTokenActivated(std::uint64_t tournamentCoinItemID, std::uint32_t dropsAwarded);
    void unsealGraffiti(std::uint64_t itemID);
    void selectTeamGraffiti(std::uint64_t itemID, std::uint16_t graffitiID);
    void statTrakSwapped(std::uint64_t itemID);
    void equipItem(std::uint64_t itemID, Team team, std::uint8_t slot);
    void markItemUpdated(std::uint64_t itemID);
};

}
