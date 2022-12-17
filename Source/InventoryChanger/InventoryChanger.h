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
    explicit InventoryChangerReturnAddresses(const helpers::PatternFinder& clientPatternFinder)
#if IS_WIN32()
    : setStickerToolSlotGetArgAsNumber{ clientPatternFinder("\xFF\xD2\xDD\x5C\x24\x10\xF2\x0F\x2C\x7C\x24").add(2).get() },
      wearItemStickerGetArgAsString{ clientPatternFinder("\xDD\x5C\x24\x18\xF2\x0F\x2C\x7C\x24?\x85\xFF").add(-80).get() },
      setNameToolStringGetArgAsString{ clientPatternFinder("\x8B\xF8\xC6\x45\x08?\x33\xC0").get() },
      clearCustomNameGetArgAsString{ clientPatternFinder("\xFF\x50\x1C\x8B\xF0\x85\xF6\x74\x21").add(3).get() },
      deleteItemGetArgAsString{ clientPatternFinder("\x85\xC0\x74\x22\x51").get() },
      setStatTrakSwapToolItemsGetArgAsString{ clientPatternFinder("\x85\xC0\x74\x7E\x8B\xC8\xE8????\x8B\x37").get() },
      acknowledgeNewItemByItemIDGetArgAsString{ clientPatternFinder("\x85\xC0\x74\x33\x8B\xC8\xE8????\xB9").get() },
      setItemAttributeValueAsyncGetArgAsString{ clientPatternFinder("\x8B\xD8\x83\xC4\x08\x85\xDB\x0F\x84????\x8B\x16\x8B\xCE\x57").add(-22).get() },
      setMyPredictionUsingItemIdGetNumArgs{ clientPatternFinder("\x8B\xF0\x89\x74\x24\x2C\x83\xFE\x01").get() },
      getMyPredictionTeamIDGetArgAsString{ clientPatternFinder("\x85\xC0\x0F\x84????\x57\x8B\xC8\xE8????\xBF????\x89\x45\xE8").add(-20).get() },
      setInventorySortAndFiltersGetArgAsString{ clientPatternFinder("\x80\x7D\xFF?\x8B\xF8\x74\x27").get() },
      getInventoryCountSetResultInt{ clientPatternFinder("\xB9????\xE8????\xB9????\xE8????\xC2\x08").add(-10).get() },
      performItemCasketTransactionGetArgAsString{ clientPatternFinder("\x85\xC0\x0F\x84????\x8B\xC8\xE8????\x52\x50\xE8????\x83\xC4\x08\x89\x44\x24\x0C\x85\xC0\x0F\x84????\xF2\x0F\x2C\x44\x24").get() },
      useToolGetArgAsString{ clientPatternFinder("\x85\xC0\x0F\x84????\x8B\xC8\xE8????\x8B\x37").get() }
#else
    : setStickerToolSlotGetArgAsNumber{ clientPatternFinder("\xF2\x44\x0F\x2C\xF0\x45\x85\xF6\x78\x32").get() },
      wearItemStickerGetArgAsString{ clientPatternFinder("\xF2\x44\x0F\x2C\xF8\x45\x39\xFE").add(-57).get() },
      setNameToolStringGetArgAsString{ clientPatternFinder("\xBA????\x4C\x89\xF6\x48\x89\xC7\x49\x89\xC4").get() },
      clearCustomNameGetArgAsString{ clientPatternFinder("\x48\x85\xC0\x74\xE5\x48\x89\xC7\xE8????\x49\x89\xC4").get() },
      deleteItemGetArgAsString{ clientPatternFinder("\x48\x85\xC0\x74\xDE\x48\x89\xC7\xE8????\x48\x89\xC3\xE8????\x48\x89\xDE").get() },
      setStatTrakSwapToolItemsGetArgAsString{ clientPatternFinder("\x74\x84\x4C\x89\xEE\x4C\x89\xF7\xE8????\x48\x85\xC0").add(-86).get() },
      acknowledgeNewItemByItemIDGetArgAsString{ clientPatternFinder("\x48\x89\xC7\xE8????\x4C\x89\xEF\x48\x89\xC6\xE8????\x48\x8B\x0B").add(-5).get() },
      setItemAttributeValueAsyncGetArgAsString{ clientPatternFinder("\xFF\x50\x38\x48\x85\xC0\x74\xC2").add(3).get() },
      setMyPredictionUsingItemIdGetNumArgs{ clientPatternFinder("\x83\xF8\x01\x89\x85").get() },
      getMyPredictionTeamIDGetArgAsString{ clientPatternFinder("\x48\x85\xC0\x74\xC5\x48\x89\xC7\x41\xBF").add(-20).get() },
      setInventorySortAndFiltersGetArgAsString{ clientPatternFinder("\x8B\x4D\xCC\x49\x89\xC5\x84\xC9").get() },
      getInventoryCountSetResultInt{ clientPatternFinder("\x48\x8B\x08\x48\x89\xDE\x48\x89\xC7\x41\x8B\x96\x38\x02").add(19).get() },
      performItemCasketTransactionGetArgAsString{ clientPatternFinder("\x48\x85\xC0\x0F\x84????\x48\x89\xC7\xE8????\x48\x89\xC7\xE8????\x48\x85\xC0\x49\x89\xC6\x0F\x84????\xF2\x0F\x10\x85").get() },
      useToolGetArgAsString{ clientPatternFinder("\x48\x85\xC0\x74\xDA\x48\x89\xC7\xE8????\x48\x8B\x0B").get() }
#endif
    {
    }

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

class InventoryChanger {
public:
    InventoryChanger(const OtherInterfaces& interfaces, const Memory& memory, game_items::Lookup gameItemLookup, game_items::CrateLootLookup crateLootLookup, const helpers::PatternFinder& clientPatternFinder, Helpers::RandomGenerator& randomGenerator)
        : backend{ std::move(gameItemLookup), std::move(crateLootLookup), memory, randomGenerator }, returnAddresses{ clientPatternFinder }, gameInventory{ interfaces, memory, clientPatternFinder } {}

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
    void onRoundMVP(const csgo::Engine& engine, const csgo::GameEvent& event);
    void updateStatTrak(const csgo::Engine& engine, const csgo::GameEvent& event);
    void overrideHudIcon(const csgo::Engine& engine, const Memory& memory, const csgo::GameEvent& event);
    void getArgAsStringHook(const Memory& memory, const char* string, ReturnAddress returnAddress, void* params);
    void getNumArgsHook(unsigned numberOfArgs, ReturnAddress returnAddress, void* params);
    int setResultIntHook(ReturnAddress returnAddress, void* params, int result);
    void onUserTextMsg(const Memory& memory, const void*& data, int& size);
    void onItemEquip(csgo::Team team, int slot, std::uint64_t& itemID);
    void acknowledgeItem(const Memory& memory, std::uint64_t itemID);
    void fixKnifeAnimation(const csgo::Entity& viewModelWeapon, long& sequence, Helpers::RandomGenerator& randomGenerator);

    void reset(const OtherInterfaces& interfaces, const Memory& memory);

    void drawGUI(const OtherInterfaces& interfaces, const Memory& memory, bool contentOnly);

    void run(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, csgo::FrameStage frameStage) noexcept;
    void scheduleHudUpdate(const OtherInterfaces& interfaces) noexcept;
    void onSoUpdated(const csgo::SharedObject& object) noexcept;

    void menuBarItem() noexcept;
    void tabItem(const OtherInterfaces& interfaces, const Memory& memory) noexcept;

    void clearItemIconTextures() noexcept;
    void clearUnusedItemIconTextures() noexcept;

private:
    void placePickEmPick(csgo::Tournament tournament, std::uint16_t group, std::uint8_t indexInGroup, csgo::StickerId stickerID);

    [[nodiscard]] auto getRequestBuilder()
    {
        return backend::RequestBuilder{ requestBuilderParams, backend.getItemIDMap(), backend.getRequestHandler(), backend.getStorageUnitHandler(), backend.getXRayScannerHandler(), backend.getItemActivationHandler() };
    }

    backend::BackendSimulator backend;
    backend::RequestBuilderParams requestBuilderParams;
    bool panoramaCodeInXrayScanner = false;
    std::vector<char> userTextMsgBuffer;
    InventoryChangerReturnAddresses returnAddresses;
    game_integration::Inventory gameInventory;
};

InventoryChanger createInventoryChanger(const OtherInterfaces& interfaces, const Memory& memory, const helpers::PatternFinder& clientPatternFinder, Helpers::RandomGenerator& randomGenerator);

}

