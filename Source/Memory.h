#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>

#include "SDK/EconItemView.h"
#include "SDK/ItemSchema.h"
#include "SDK/MoveHelper.h"
#include "SDK/PanoramaMarshallHelper.h"
#include "SDK/Platform.h"
#include "SDK/ViewRenderBeams.h"
#include "SDK/WeaponSystem.h"

#include "Interfaces.h"

#include "SafeAddress.h"
#include "RetSpoofGadgets.h"
#include "Helpers/PatternFinder.h"

class ClientMode;
class ClientSharedObjectCache;
class CSPlayerInventory;
class EconItem;
class EconItemAttributeDefinition;
class Entity;
class GameEventDescriptor;
class GameEventManager;
class Input;
class KeyValues;
class MoveData;
class PlantedC4;
class PlayerResource;
class ViewRender;
template <typename Key, typename Value>
struct UtlMap;
template <typename T>
class UtlVector;

struct ActiveChannels;
struct Channel;
struct GlobalVars;
struct GlowObjectManager;
struct PanoramaEventRegistration;
struct Vector;

namespace csgo::pod
{
    struct EconItem;
    struct ItemSystem;
}

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

    std::uintptr_t setStickerToolSlotGetArgAsNumber;
    std::uintptr_t wearItemStickerGetArgAsString;
    std::uintptr_t setNameToolStringGetArgAsString;
    std::uintptr_t clearCustomNameGetArgAsString;
    std::uintptr_t deleteItemGetArgAsString;
    std::uintptr_t setStatTrakSwapToolItemsGetArgAsString;
    std::uintptr_t acknowledgeNewItemByItemIDGetArgAsString;
    std::uintptr_t setItemAttributeValueAsyncGetArgAsString;
    std::uintptr_t setMyPredictionUsingItemIdGetNumArgs;
    std::uintptr_t getMyPredictionTeamIDGetArgAsString;
    std::uintptr_t setInventorySortAndFiltersGetArgAsString;
    std::uintptr_t getInventoryCountSetResultInt;
    std::uintptr_t performItemCasketTransactionGetArgAsString;
    std::uintptr_t useToolGetArgAsString;
};

class Memory {
public:
    Memory(const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder, std::uintptr_t clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

#if IS_WIN32()
    std::uintptr_t present;
    std::uintptr_t reset;
#endif

    ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;
    UtlVector<PlantedC4*>* plantedC4s;
    UtlMap<short, PanoramaEventRegistration>* registeredPanoramaEvents;

    bool* disablePostProcessing;

    std::add_pointer_t<void FASTCALL_CONV(const char*)> loadSky;
    std::add_pointer_t<void FASTCALL_CONV(const char*, const char*)> setClanTag;
    std::uintptr_t cameraThink;
    std::add_pointer_t<bool CDECL_CONV(Vector, Vector, short)> lineGoesThroughSmoke;
    int(THISCALL_CONV* getSequenceActivity)(void*, int);
    bool(THISCALL_CONV* isOtherEnemy)(std::uintptr_t, std::uintptr_t);
    std::uintptr_t hud;
    int*(THISCALL_CONV* findHudElement)(std::uintptr_t, const char*);
    int(THISCALL_CONV* clearHudWeapon)(int*, int);
    void(THISCALL_CONV* setAbsOrigin)(std::uintptr_t, const Vector&);
    std::uintptr_t insertIntoTree;
    int* dispatchSound;
    std::uintptr_t traceToExit;
    ViewRender* viewRender;
    ViewRenderBeams viewRenderBeams;
    std::uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<void CDECL_CONV(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void CDECL_CONV(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    float* vignette;
    int(THISCALL_CONV* equipWearable)(csgo::pod::Entity* wearable, csgo::pod::Entity* player);
    int* predictionRandomSeed;
    MoveData* moveData;
    std::uintptr_t keyValuesFromString;
    KeyValues*(THISCALL_CONV* keyValuesFindKey)(KeyValues* keyValues, const char* keyName, bool create);
    void(THISCALL_CONV* keyValuesSetString)(KeyValues* keyValues, const char* value);
    WeaponSystem weaponSystem;
    std::add_pointer_t<const char** FASTCALL_CONV(const char* playerModelName)> getPlayerViewmodelArmConfigForPlayerModel;
    GameEventDescriptor* (THISCALL_CONV* getEventDescriptor)(/* GameEventManager* */ std::uintptr_t thisptr, const char* name, int* cookie);
    ActiveChannels* activeChannels;
    Channel* channels;
    PlayerResource** playerResource;
    const wchar_t*(THISCALL_CONV* getDecoratedPlayerName)(PlayerResource* pr, int index, wchar_t* buffer, int buffsize, int flags);
    std::uintptr_t scopeDust;
    std::uintptr_t scopeArc;
    std::uintptr_t demoOrHLTV;
    std::uintptr_t money;
    std::uintptr_t demoFileEndReached;
    csgo::pod::Entity** gameRules;
    InventoryManager inventoryManager;
    std::add_pointer_t<csgo::pod::EconItem* STDCALL_CONV()> createEconItemSharedObject;
    bool(THISCALL_CONV* addEconItem)(std::uintptr_t playerInventory, csgo::pod::EconItem* item, bool updateAckFile, bool writeAckFile, bool checkForNewItems);
    void(THISCALL_CONV* clearInventoryImageRGBA)(csgo::pod::EconItemView* itemView);
    csgo::pod::PanoramaMarshallHelper* panoramaMarshallHelper;
    InventoryChangerReturnAddresses inventoryChangerReturnAddresses;
    std::add_pointer_t<csgo::pod::EconItemView* CDECL_CONV(std::uint64_t itemID)> findOrCreateEconItemViewForItemID;
    void*(THISCALL_CONV* getInventoryItemByItemID)(std::uintptr_t playerInventory, std::uint64_t itemID);
    csgo::pod::EconItem*(THISCALL_CONV* getSOCData)(void* itemView);
    void(THISCALL_CONV* setCustomName)(csgo::pod::EconItem* thisptr, const char* name);
    std::uintptr_t createBaseTypeCache;
    void** uiComponentInventory;
    void(THISCALL_CONV* setItemSessionPropertyValue)(void* thisptr, std::uint64_t itemID, const char* type, const char* value);
    std::uintptr_t removeDynamicAttribute;

    short makePanoramaSymbol(const char* name) const noexcept
    {
        short symbol;
        makePanoramaSymbolFn(&symbol, name);
        return symbol;
    }

    bool submitReport(const char* xuid, const char* report) const noexcept
    {
#if IS_WIN32()
        return reinterpret_cast<bool(__stdcall*)(const char*, const char*)>(submitReportFunction)(xuid, report);
#else
        return reinterpret_cast<bool(*)(void*, const char*, const char*)>(submitReportFunction)(nullptr, xuid, report);
#endif
    }

    [[nodiscard]] ItemSystem itemSystem() const noexcept
    {
        return ItemSystem::from(retSpoofGadgets->client, itemSystemFn());
    }

    [[nodiscard]] MoveHelper moveHelper() const noexcept
    {
        return MoveHelper::from(retSpoofGadgets->client, moveHelperPtr);
    }

#if IS_WIN32()
    class KeyValuesSystem* keyValuesSystem;
    std::uintptr_t keyValuesAllocEngine;
    std::uintptr_t keyValuesAllocClient;

    std::uintptr_t shouldDrawFogReturnAddress;
#endif
    std::uintptr_t setDynamicAttributeValueFn;

private:
    void(THISCALL_CONV* makePanoramaSymbolFn)(short* symbol, const char* name);
    std::add_pointer_t<csgo::pod::ItemSystem* CDECL_CONV()> itemSystemFn;

    std::uintptr_t submitReportFunction;
    csgo::pod::MoveHelper* moveHelperPtr;
};

inline std::optional<const Memory> memory;
