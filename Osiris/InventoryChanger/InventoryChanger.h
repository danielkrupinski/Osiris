#pragma once

#include <cstdint>

#include "InventoryConfig.h"

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

    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;
    void onRoundMVP(GameEvent& event) noexcept;

    void deleteItem(std::uint64_t itemID) noexcept;
    void acknowledgeItem(std::uint64_t itemID) noexcept;

    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;

    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept;

    void clearInventory() noexcept;

    void onItemEquip(Team team, int slot, std::uint64_t itemID) noexcept;
    void onSoUpdated(SharedObject* object) noexcept;
    void onUserTextMsg(const void*& data, int& size) noexcept;
    void getArgAsStringHook(const char* string, std::uintptr_t returnAddress) noexcept;
    void getArgAsNumberHook(int number, std::uintptr_t returnAddress) noexcept;
}
