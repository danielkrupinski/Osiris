#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "Backend/BackendSimulator.h"
#include "Backend/Request/RequestBuilder.h"
#include "Backend/Request/ItemActivationHandler.h"
#include "Backend/Request/XRayScannerHandler.h"
#include <BytePatterns/ClientPatternFinder.h>
#include "GameIntegration/Inventory.h"
#include "GameItems/Lookup.h"
#include "GameItems/CrateLootLookup.h"
#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/EngineInterfaces.h>
#include <Interfaces/OtherInterfaces.h>
#include <MemorySearch/BytePatternLiteral.h>
#include "EconItemFunctions.h"
#include "EconItemViewFunctions.h"
#include <Utils/ReturnAddress.h>

namespace csgo
{

enum class FrameStage;
enum class Team;
struct Entity;
struct EntityPOD;
struct GameEvent;
struct SharedObject;

}

class ClientInterfaces;
class PanoramaMarshallHelperHooks;

namespace inventory_changer
{

struct InventoryChangerReturnAddresses {
    ReturnAddress setStickerToolSlotGetArgAsNumber;
    ReturnAddress wearItemStickerGetArgAsString;
    ReturnAddress setNameToolStringGetArgAsString;
    ReturnAddress clearCustomNameGetArgAsString;
    ReturnAddress deleteItemGetArgAsString;
    ReturnAddress setStatTrakSwapToolItemsGetArgAsString;
    ReturnAddress acknowledgeNewItemByItemIDGetArgAsString;
    ReturnAddress setItemAttributeValueAsyncGetArgAsString;
    ReturnAddress setMyPredictionUsingItemIdGetNumArgs;
    ReturnAddress getMyPredictionTeamIDGetArgAsString;
    ReturnAddress setInventorySortAndFiltersGetArgAsString;
    ReturnAddress getInventoryCountSetResultInt;
    ReturnAddress performItemCasketTransactionGetArgAsString;
    ReturnAddress useToolGetArgAsString;
};

[[nodiscard]] InventoryChangerReturnAddresses createInventoryChangerReturnAddresses(const ClientPatternFinder& clientPatternFinder);

class InventoryChanger {
public:
    InventoryChanger(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, game_items::Lookup gameItemLookup, game_items::CrateLootLookup crateLootLookup, const ClientPatternFinder& clientPatternFinder, Helpers::RandomGenerator& randomGenerator)
        : engineInterfaces{ engineInterfaces }, clientInterfaces{ clientInterfaces }, otherInterfaces{ interfaces }, backend{ std::move(gameItemLookup), std::move(crateLootLookup), memory, randomGenerator }, returnAddresses{ createInventoryChangerReturnAddresses(clientPatternFinder) }, gameInventory{ interfaces, memory, clientPatternFinder } {}

    [[nodiscard]] const game_items::Lookup& getGameItemLookup() const noexcept
    {
        return backend.getGameItemLookup();
    }

    [[nodiscard]] const game_items::CrateLootLookup& getCrateLootLookup() const noexcept
    {
        return backend.getCrateLootLookup();
    }

    [[nodiscard]] const backend::BackendSimulator& getBackend() const noexcept
    {
        return backend;
    }

    [[nodiscard]] backend::BackendSimulator& getBackend() noexcept
    {
        return backend;
    }

    void getArgAsNumberHook(int number, ReturnAddress returnAddress);
    void onRoundMVP(const csgo::GameEvent& event);
    void updateStatTrak(const csgo::GameEvent& event);
    void overrideHudIcon(const Memory& memory, const csgo::GameEvent& event);
    void getArgAsStringHook(const PanoramaMarshallHelperHooks& panoramaMarshallHelperHooks, const Memory& memory, const char* string, ReturnAddress returnAddress, void* params);
    void getNumArgsHook(const PanoramaMarshallHelperHooks& panoramaMarshallHelperHooks, csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper, unsigned numberOfArgs, ReturnAddress returnAddress, void* params);
    int setResultIntHook(ReturnAddress returnAddress, void* params, int result);
    void onUserTextMsg(const Memory& memory, const void*& data, int& size);
    void onItemEquip(csgo::Team team, int slot, std::uint64_t& itemID);
    void acknowledgeItem(const Memory& memory, std::uint64_t itemID);
    void fixKnifeAnimation(const csgo::Entity& viewModelWeapon, long& sequence, Helpers::RandomGenerator& randomGenerator);

    void reset(const Memory& memory);

    void drawGUI(const Memory& memory, bool contentOnly);

    void run(const Memory& memory, csgo::FrameStage frameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void onSoUpdated(const csgo::SharedObject& object) noexcept;

    void menuBarItem() noexcept;
    void tabItem(const Memory& memory) noexcept;

    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;

private:
    void placePickEmPick(csgo::Tournament tournament, std::uint16_t group, std::uint8_t indexInGroup, csgo::StickerId stickerID);

    [[nodiscard]] auto getRequestBuilder()
    {
        return backend::RequestBuilder{ requestBuilderParams, backend.getItemIDMap(), backend.getRequestHandler(), backend.getStorageUnitHandler(), backend.getXRayScannerHandler(), backend.getItemActivationHandler() };
    }

    EngineInterfaces engineInterfaces;
    ClientInterfaces clientInterfaces;
    OtherInterfaces otherInterfaces;
    backend::BackendSimulator backend;
    backend::RequestBuilderParams requestBuilderParams;
    bool panoramaCodeInXrayScanner = false;
    std::vector<char> userTextMsgBuffer;
    InventoryChangerReturnAddresses returnAddresses;
    game_integration::Inventory gameInventory;
};

InventoryChanger createInventoryChanger(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const ClientPatternFinder& clientPatternFinder, Helpers::RandomGenerator& randomGenerator);

}
