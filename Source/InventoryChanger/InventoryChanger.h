#pragma once

#include <cstdint>

#include "InventoryConfig.h"

#include "Backend/BackendSimulator.h"
#include "Backend/Request/RequestBuilder.h"
#include "GameItems/Lookup.h"
#include "GameItems/CrateLootLookup.h"

enum class FrameStage;
enum class Team;
class Entity;
class GameEvent;
class SharedObject;

namespace inventory_changer
{

class InventoryChanger {
public:
    InventoryChanger(game_items::Lookup gameItemLookup, game_items::CrateLootLookup crateLootLookup)
        : gameItemLookup{ std::move(gameItemLookup) }, crateLootLookup{ std::move(crateLootLookup) }, backend{ this->gameItemLookup, this->crateLootLookup }, backendRequestBuilder{ backend } {}

    static InventoryChanger& instance();

    [[nodiscard]] const game_items::Lookup& getGameItemLookup() const noexcept
    {
        return gameItemLookup;
    }

    [[nodiscard]] const game_items::CrateLootLookup& getCrateLootLookup() const noexcept
    {
        return crateLootLookup;
    }

    [[nodiscard]] const backend::BackendSimulator& getBackend() const noexcept
    {
        return backend;
    }

    [[nodiscard]] backend::BackendSimulator& getBackend() noexcept
    {
        return backend;
    }

    [[nodiscard]] backend::RequestBuilder& getBackendRequestBuilder() noexcept
    {
        return backendRequestBuilder;
    }

    void getArgAsNumberHook(int number, std::uintptr_t returnAddress);

private:
    game_items::Lookup gameItemLookup;
    game_items::CrateLootLookup crateLootLookup;
    backend::BackendSimulator backend;
    backend::RequestBuilder backendRequestBuilder;
};

}

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

    void acknowledgeItem(std::uint64_t itemID) noexcept;

    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;

    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept;

    void clearInventory() noexcept;

    void onItemEquip(Team team, int slot, std::uint64_t& itemID) noexcept;
    void onSoUpdated(SharedObject* object) noexcept;
    void onUserTextMsg(const void*& data, int& size) noexcept;
    void getArgAsStringHook(const char* string, std::uintptr_t returnAddress, void* params) noexcept;
    void getNumArgsHook(unsigned numberOfArgs, std::uintptr_t returnAddress, void* params) noexcept;
}
