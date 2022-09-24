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

    static InventoryChanger& instance(const Interfaces& interfaces, const Memory& memory);

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

    void getArgAsNumberHook(const Memory& memory, int number, std::uintptr_t returnAddress);
    void onRoundMVP(const Interfaces& interfaces, GameEvent& event);
    void updateStatTrak(const Interfaces& interfaces, GameEvent& event);
    void overrideHudIcon(const Interfaces& interfaces, const Memory& memory, GameEvent& event);
    void getArgAsStringHook(const Memory& memory, const char* string, std::uintptr_t returnAddress, void* params);
    void getNumArgsHook(const Memory& memory, unsigned numberOfArgs, std::uintptr_t returnAddress, void* params);
    int setResultIntHook(const Memory& memory, std::uintptr_t returnAddress, void* params, int result);
    void onUserTextMsg(const Memory& memory, const void*& data, int& size);
    void onItemEquip(csgo::Team team, int slot, std::uint64_t& itemID);
    void acknowledgeItem(const Memory& memory, std::uint64_t itemID);
    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence);

    void reset(const Interfaces& interfaces, const Memory& memory);

    void drawGUI(const Interfaces& interfaces, const Memory& memory, bool contentOnly);

    void run(const Interfaces& interfaces, const Memory& memory, csgo::FrameStage frameStage) noexcept;
    void scheduleHudUpdate(const Interfaces& interfaces) noexcept;
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
    void tabItem(const Interfaces& interfaces, const Memory& memory) noexcept;

    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;
}
