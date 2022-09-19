#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "Backend/BackendSimulator.h"
#include "Backend/Request/RequestBuilder.h"
#include "Backend/Request/ItemActivationHandler.h"
#include "Backend/Request/XRayScannerHandler.h"
#include "GameItems/Lookup.h"
#include "GameItems/CrateLootLookup.h"

namespace csgo { enum class FrameStage; }
enum class Team;
class Entity;
class GameEvent;
class SharedObject;

namespace inventory_changer
{

class InventoryChanger {
public:
    InventoryChanger(game_items::Lookup gameItemLookup, game_items::CrateLootLookup crateLootLookup)
        : gameItemLookup{ std::move(gameItemLookup) }, crateLootLookup{ std::move(crateLootLookup) }, backend{ this->gameItemLookup, this->crateLootLookup } {}

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

    void getArgAsNumberHook(int number, std::uintptr_t returnAddress);
    void onRoundMVP(GameEvent& event);
    void updateStatTrak(GameEvent& event);
    void overrideHudIcon(GameEvent& event);
    void getArgAsStringHook(const char* string, std::uintptr_t returnAddress, void* params);
    void getNumArgsHook(unsigned numberOfArgs, std::uintptr_t returnAddress, void* params);
    int setResultIntHook(std::uintptr_t returnAddress, void* params, int result);
    void onUserTextMsg(const void*& data, int& size);
    void onItemEquip(csgo::Team team, int slot, std::uint64_t& itemID);
    void acknowledgeItem(std::uint64_t itemID);
    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence);

    void reset();

    void drawGUI(bool contentOnly);

    void run(csgo::FrameStage frameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void onSoUpdated(SharedObject* object) noexcept;

private:
    void placePickEmPick(csgo::Tournament tournament, std::uint16_t group, std::uint8_t indexInGroup, csgo::StickerId stickerID);

    [[nodiscard]] auto getRequestBuilder()
    {
        return backend::RequestBuilder{ requestBuilderParams, backend.getItemIDMap(), backend.getRequestHandler(), backend.getStorageUnitHandler(), backend.getXRayScannerHandler(), backend.getItemActivationHandler() };
    }

    game_items::Lookup gameItemLookup;
    game_items::CrateLootLookup crateLootLookup;
    backend::BackendSimulator backend;
    backend::RequestBuilderParams requestBuilderParams;
    bool panoramaCodeInXrayScanner = false;
    std::vector<char> userTextMsgBuffer;
};

}

namespace InventoryChanger
{
    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;

    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;
}
