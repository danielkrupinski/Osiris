#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "Backend/BackendSimulator.h"
#include "Backend/Request/RequestBuilder.h"
#include "Backend/Request/ItemActivationHandler.h"
#include "Backend/Request/XRayScannerHandler.h"
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
class Entity;
struct EntityPOD;
class GameEvent;
class SharedObject;

}

class ClientInterfaces;

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

[[nodiscard]] inline InventoryChangerReturnAddresses createInventoryChangerReturnAddresses(const helpers::PatternFinder& clientPatternFinder)
{
    return InventoryChangerReturnAddresses{
#if IS_WIN32()
        .setStickerToolSlotGetArgAsNumber = clientPatternFinder("FF D2 DD 5C 24 10 F2 0F 2C 7C 24"_pat, OffsetHint{ 6120862 }).add(2).asReturnAddress(),
        .wearItemStickerGetArgAsString = clientPatternFinder("DD 5C 24 18 F2 0F 2C 7C 24 ? 85 FF"_pat, OffsetHint{ 6095215 }).add(-80).asReturnAddress(),
        .setNameToolStringGetArgAsString = clientPatternFinder("8B F8 C6 45 08 ? 33 C0"_pat, OffsetHint{ 6121111 }).asReturnAddress(),
        .clearCustomNameGetArgAsString = clientPatternFinder("FF 50 1C 8B F0 85 F6 74 21"_pat, OffsetHint{ 6121269 }).add(3).asReturnAddress(),
        .deleteItemGetArgAsString = clientPatternFinder("85 C0 74 22 51"_pat, OffsetHint{ 6122298 }).asReturnAddress(),
        .setStatTrakSwapToolItemsGetArgAsString = clientPatternFinder("85 C0 74 7E 8B C8 E8 ? ? ? ? 8B 37"_pat, OffsetHint{ 6120623 }).asReturnAddress(),
        .acknowledgeNewItemByItemIDGetArgAsString = clientPatternFinder("85 C0 74 33 8B C8 E8 ? ? ? ? B9"_pat, OffsetHint{ 6092121 }).asReturnAddress(),
        .setItemAttributeValueAsyncGetArgAsString = clientPatternFinder("8B D8 83 C4 08 85 DB 0F 84 ? ? ? ? 8B 16 8B CE 57"_pat, OffsetHint{ 6098822 }).add(-22).asReturnAddress(),
        .setMyPredictionUsingItemIdGetNumArgs = clientPatternFinder("8B F0 89 74 24 2C 83 FE 01"_pat, OffsetHint{ 6702914 }).asReturnAddress(),
        .getMyPredictionTeamIDGetArgAsString = clientPatternFinder("85 C0 0F 84 ? ? ? ? 57 8B C8 E8 ? ? ? ? BF ? ? ? ? 89 45 E8"_pat, OffsetHint{ 6699829 }).add(-20).asReturnAddress(),
        .setInventorySortAndFiltersGetArgAsString = clientPatternFinder("80 7D FF ? 8B F8 74 27"_pat, OffsetHint{ 6088445 }).asReturnAddress(),
        .getInventoryCountSetResultInt = clientPatternFinder("B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? C2 08"_pat, OffsetHint{ 6092048 }).add(-10).asReturnAddress(),
        .performItemCasketTransactionGetArgAsString = clientPatternFinder("85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 52 50 E8 ? ? ? ? 83 C4 08 89 44 24 0C 85 C0 0F 84 ? ? ? ? F2 0F 2C 44 24"_pat, OffsetHint{ 6098213 }).asReturnAddress(),
        .useToolGetArgAsString = clientPatternFinder("85 C0 0F 84 ? ? ? ? 8B C8 E8 ? ? ? ? 8B 37"_pat, OffsetHint{ 6118723 }).asReturnAddress()
#elif IS_LINUX()
        .setStickerToolSlotGetArgAsNumber = clientPatternFinder("F2 44 0F 2C F0 45 85 F6 78 32"_pat, OffsetHint{ 11247132 }).asReturnAddress(),
        .wearItemStickerGetArgAsString = clientPatternFinder("F2 44 0F 2C F8 45 39 FE"_pat, OffsetHint{ 11225783 }).add(-57).asReturnAddress(),
        .setNameToolStringGetArgAsString = clientPatternFinder("BA ? ? ? ? 4C 89 F6 48 89 C7 49 89 C4"_pat, OffsetHint{ 11252637 }).asReturnAddress(),
        .clearCustomNameGetArgAsString = clientPatternFinder("48 85 C0 74 E5 48 89 C7 E8 ? ? ? ? 49 89 C4"_pat, OffsetHint{ 11247366 }).asReturnAddress(),
        .deleteItemGetArgAsString = clientPatternFinder("48 85 C0 74 DE 48 89 C7 E8 ? ? ? ? 48 89 C3 E8 ? ? ? ? 48 89 DE"_pat, OffsetHint{ 11248886 }).asReturnAddress(),
        .setStatTrakSwapToolItemsGetArgAsString = clientPatternFinder("74 84 4C 89 EE 4C 89 F7 E8 ? ? ? ? 48 85 C0"_pat, OffsetHint{ 11246916 }).add(-86).asReturnAddress(),
        .acknowledgeNewItemByItemIDGetArgAsString = clientPatternFinder("48 89 C7 E8 ? ? ? ? 4C 89 EF 48 89 C6 E8 ? ? ? ? 48 8B 0B"_pat, OffsetHint{ 11221963 }).add(-5).asReturnAddress(),
        .setItemAttributeValueAsyncGetArgAsString = clientPatternFinder("FF 50 38 48 85 C0 74 C2"_pat, OffsetHint{ 11228494 }).add(3).asReturnAddress(),
        .setMyPredictionUsingItemIdGetNumArgs = clientPatternFinder("83 F8 01 89 85"_pat, OffsetHint{ 11613975 }).asReturnAddress(),
        .getMyPredictionTeamIDGetArgAsString = clientPatternFinder("48 85 C0 74 C5 48 89 C7 41 BF"_pat, OffsetHint{ 11606770 }).add(-20).asReturnAddress(),
        .setInventorySortAndFiltersGetArgAsString = clientPatternFinder("8B 4D CC 49 89 C5 84 C9"_pat, OffsetHint{ 11312157 }).asReturnAddress(),
        .getInventoryCountSetResultInt = clientPatternFinder("48 8B 08 48 89 DE 48 89 C7 41 8B 96 38 02"_pat, OffsetHint{ 11404345 }).add(19).asReturnAddress(),
        .performItemCasketTransactionGetArgAsString = clientPatternFinder("48 85 C0 0F 84 ? ? ? ? 48 89 C7 E8 ? ? ? ? 48 89 C7 E8 ? ? ? ? 48 85 C0 49 89 C6 0F 84 ? ? ? ? F2 0F 10 85"_pat, OffsetHint{ 11300702 }).asReturnAddress(),
        .useToolGetArgAsString = clientPatternFinder("48 85 C0 74 DA 48 89 C7 E8 ? ? ? ? 48 8B 0B"_pat, OffsetHint{ 11246382 }).asReturnAddress()
#endif
    };
}

class InventoryChanger {
public:
    InventoryChanger(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, game_items::Lookup gameItemLookup, game_items::CrateLootLookup crateLootLookup, const helpers::PatternFinder& clientPatternFinder, Helpers::RandomGenerator& randomGenerator)
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
    void getArgAsStringHook(const Memory& memory, const char* string, ReturnAddress returnAddress, void* params);
    void getNumArgsHook(csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper, unsigned numberOfArgs, ReturnAddress returnAddress, void* params);
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

InventoryChanger createInventoryChanger(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const helpers::PatternFinder& clientPatternFinder, Helpers::RandomGenerator& randomGenerator);

}

