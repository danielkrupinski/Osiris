#pragma once

#include <cstdint>

#include "../JsonForward.h"

enum class FrameStage;
enum class Team;
class Entity;
class GameEvent;
class SharedObject;

namespace InventoryChanger
{
    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;

    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;
    void onRoundMVP(GameEvent& event) noexcept;

    void setStickerApplySlot(int slot) noexcept;
    void setToolToUse(std::uint64_t itemID) noexcept;
    void setItemToApplyTool(std::uint64_t itemID) noexcept;
    void setStickerSlotToWear(int slot) noexcept;
    void setItemToWearSticker(std::uint64_t itemID) noexcept;
    void setNameTagString(const char* str) noexcept;
    void setItemToRemoveNameTag(std::uint64_t itemID) noexcept;
    void setStatTrakSwapItem1(std::uint64_t itemID) noexcept;
    void setStatTrakSwapItem2(std::uint64_t itemID) noexcept;
    void deleteItem(std::uint64_t itemID) noexcept;
    void acknowledgeItem(std::uint64_t itemID) noexcept;

    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;

    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept;

    void clearInventory() noexcept;

    void onItemEquip(Team team, int slot, std::uint64_t itemID) noexcept;
    void onSoUpdated(SharedObject* object) noexcept;
    void onUserTextMsg(const void*& data, int& size) noexcept;
}
