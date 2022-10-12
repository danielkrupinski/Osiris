#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>

#include "SDK/Platform.h"
#include "SDK/ViewRenderBeams.h"
#include "SDK/WeaponSystem.h"

#include "Interfaces.h"

#include "SafeAddress.h"
#include "RetSpoofGadgets.h"

class ClientMode;
template <typename T> class ClientSharedObjectCache;
class CSPlayerInventory;
class EconItem;
class EconItemAttributeDefinition;
class EconItemView;
class Entity;
class GameEventDescriptor;
class GameEventManager;
class Input;
class ItemSystem;
class InventoryManager;
class KeyValues;
class MoveHelper;
class MoveData;
class PanoramaMarshallHelper;
class PlantedC4;
class PlayerResource;
template <typename T> class SharedObjectTypeCache;
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

template <bool ReportNotFound = true>
std::uintptr_t findPattern(const char* moduleName, std::string_view pattern) noexcept;

struct InventoryChangerReturnAddresses {
    InventoryChangerReturnAddresses()
#ifdef _WIN32
    : setStickerToolSlotGetArgAsNumber{ SafeAddress{ findPattern(CLIENT_DLL, "\xFF\xD2\xDD\x5C\x24\x10\xF2\x0F\x2C\x7C\x24") }.add(2).get() },
      wearItemStickerGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\xDD\x5C\x24\x18\xF2\x0F\x2C\x7C\x24?\x85\xFF") }.add(-80).get() },
      setNameToolStringGetArgAsString{ findPattern(CLIENT_DLL, "\x8B\xF8\xC6\x45\x08?\x33\xC0") },
      clearCustomNameGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\xFF\x50\x1C\x8B\xF0\x85\xF6\x74\x21") }.add(3).get() },
      deleteItemGetArgAsString{ findPattern(CLIENT_DLL, "\x85\xC0\x74\x22\x51") },
      setStatTrakSwapToolItemsGetArgAsString{ findPattern(CLIENT_DLL, "\x85\xC0\x74\x7E\x8B\xC8\xE8????\x8B\x37") },
      acknowledgeNewItemByItemIDGetArgAsString{ findPattern(CLIENT_DLL, "\x85\xC0\x74\x33\x8B\xC8\xE8????\xB9") },
      setItemAttributeValueAsyncGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\x8B\xD8\x83\xC4\x08\x85\xDB\x0F\x84????\x8B\x16\x8B\xCE\x57") }.add(-22).get() },
      setMyPredictionUsingItemIdGetNumArgs{ findPattern(CLIENT_DLL, "\x8B\xF0\x89\x74\x24\x2C\x83\xFE\x01") },
      getMyPredictionTeamIDGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\x85\xC0\x0F\x84????\x57\x8B\xC8\xE8????\xBF????\x89\x45\xE8") }.add(-20).get() },
      setInventorySortAndFiltersGetArgAsString{ findPattern(CLIENT_DLL, "\x80\x7D\xFF?\x8B\xF8\x74\x27") },
      getInventoryCountSetResultInt{ SafeAddress{ findPattern(CLIENT_DLL, "\xB9????\xE8????\xB9????\xE8????\xC2\x08") }.add(-10).get() },
      performItemCasketTransactionGetArgAsString{ findPattern(CLIENT_DLL, "\x85\xC0\x0F\x84????\x8B\xC8\xE8????\x52\x50\xE8????\x83\xC4\x08\x89\x44\x24\x0C\x85\xC0\x0F\x84????\xF2\x0F\x2C\x44\x24") },
      useToolGetArgAsString{ findPattern(CLIENT_DLL, "\x85\xC0\x0F\x84????\x8B\xC8\xE8????\x8B\x37") }
#else
    : setStickerToolSlotGetArgAsNumber{ findPattern(CLIENT_DLL, "\xF2\x44\x0F\x2C\xF0\x45\x85\xF6\x78\x32") },
      wearItemStickerGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\xF2\x44\x0F\x2C\xF8\x45\x39\xFE") }.add(-57).get() },
      setNameToolStringGetArgAsString{ findPattern(CLIENT_DLL, "\xBA????\x4C\x89\xF6\x48\x89\xC7\x49\x89\xC4") },
      clearCustomNameGetArgAsString{ findPattern(CLIENT_DLL, "\x48\x85\xC0\x74\xE5\x48\x89\xC7\xE8????\x49\x89\xC4") },
      deleteItemGetArgAsString{ findPattern(CLIENT_DLL, "\x48\x85\xC0\x74\xDE\x48\x89\xC7\xE8????\x48\x89\xC3\xE8????\x48\x89\xDE") },
      setStatTrakSwapToolItemsGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\x74\x84\x4C\x89\xEE\x4C\x89\xF7\xE8????\x48\x85\xC0") }.add(-86).get() },
      acknowledgeNewItemByItemIDGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\x48\x89\xC7\xE8????\x4C\x89\xEF\x48\x89\xC6\xE8????\x48\x8B\x0B") }.add(-5).get() },
      setItemAttributeValueAsyncGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\xFF\x50\x38\x48\x85\xC0\x74\xC2") }.add(3).get() },
      setMyPredictionUsingItemIdGetNumArgs{ findPattern(CLIENT_DLL, "\x83\xF8\x01\x89\x85") },
      getMyPredictionTeamIDGetArgAsString{ SafeAddress{ findPattern(CLIENT_DLL, "\x48\x85\xC0\x74\xC5\x48\x89\xC7\x41\xBF") }.add(-20).get() },
      setInventorySortAndFiltersGetArgAsString{ findPattern(CLIENT_DLL, "\x8B\x4D\xCC\x49\x89\xC5\x84\xC9") },
      getInventoryCountSetResultInt{ SafeAddress{ findPattern(CLIENT_DLL, "\x48\x8B\x08\x48\x89\xDE\x48\x89\xC7\x41\x8B\x96\x38\x02") }.add(19).get() },
      performItemCasketTransactionGetArgAsString{ findPattern(CLIENT_DLL, "\x48\x85\xC0\x0F\x84????\x48\x89\xC7\xE8????\x48\x89\xC7\xE8????\x48\x85\xC0\x49\x89\xC6\x0F\x84????\xF2\x0F\x10\x85") },
      useToolGetArgAsString{ findPattern(CLIENT_DLL, "\x48\x85\xC0\x74\xDA\x48\x89\xC7\xE8????\x48\x8B\x0B") }
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
    Memory(std::uintptr_t clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

#ifdef _WIN32
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
    bool(THISCALL_CONV* isOtherEnemy)(Entity*, Entity*);
    std::uintptr_t hud;
    int*(THISCALL_CONV* findHudElement)(std::uintptr_t, const char*);
    int(THISCALL_CONV* clearHudWeapon)(int*, int);
    std::add_pointer_t<ItemSystem* CDECL_CONV()> itemSystem;
    void(THISCALL_CONV* setAbsOrigin)(Entity*, const Vector&);
    std::uintptr_t insertIntoTree;
    int* dispatchSound;
    std::uintptr_t traceToExit;
    ViewRender* viewRender;
    ViewRenderBeams viewRenderBeams;
    std::uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<void CDECL_CONV(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void CDECL_CONV(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    float* vignette;
    int(THISCALL_CONV* equipWearable)(void* wearable, void* player);
    int* predictionRandomSeed;
    MoveData* moveData;
    MoveHelper* moveHelper;
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
    Entity** gameRules;
    InventoryManager* inventoryManager;
    std::add_pointer_t<EconItem* STDCALL_CONV()> createEconItemSharedObject;
    bool(THISCALL_CONV* addEconItem)(CSPlayerInventory* thisptr, EconItem* item, bool updateAckFile, bool writeAckFile, bool checkForNewItems);
    void(THISCALL_CONV* clearInventoryImageRGBA)(EconItemView* itemView);
    PanoramaMarshallHelper* panoramaMarshallHelper;
    InventoryChangerReturnAddresses inventoryChangerReturnAddresses;
    std::add_pointer_t<EconItemView* CDECL_CONV(std::uint64_t itemID)> findOrCreateEconItemViewForItemID;
    void*(THISCALL_CONV* getInventoryItemByItemID)(CSPlayerInventory* thisptr, std::uint64_t itemID);
    EconItem*(THISCALL_CONV* getSOCData)(void* itemView);
    void(THISCALL_CONV* setCustomName)(EconItem* thisptr, const char* name);
    SharedObjectTypeCache<EconItem>*(THISCALL_CONV* createBaseTypeCache)(ClientSharedObjectCache<EconItem>* thisptr, int classID);
    void** uiComponentInventory;
    void(THISCALL_CONV* setItemSessionPropertyValue)(void* thisptr, std::uint64_t itemID, const char* type, const char* value);
    void(THISCALL_CONV* removeDynamicAttribute)(EconItem* thisptr, EconItemAttributeDefinition* attribute);

    short makePanoramaSymbol(const char* name) const noexcept
    {
        short symbol;
        makePanoramaSymbolFn(&symbol, name);
        return symbol;
    }

    bool submitReport(const char* xuid, const char* report) const noexcept
    {
#ifdef _WIN32
        return reinterpret_cast<bool(__stdcall*)(const char*, const char*)>(submitReportFunction)(xuid, report);
#else
        return reinterpret_cast<bool(*)(void*, const char*, const char*)>(submitReportFunction)(nullptr, xuid, report);
#endif
    }

    void setDynamicAttributeValue(EconItem* thisptr, EconItemAttributeDefinition* attribute, void* value) const noexcept
    {
#ifdef _WIN32
        reinterpret_cast<void(__thiscall*)(EconItem*, EconItemAttributeDefinition*, void*)>(setDynamicAttributeValueFn)(thisptr, attribute, value);
#else
        reinterpret_cast<void(*)(void*, EconItem*, EconItemAttributeDefinition*, void*)>(setDynamicAttributeValueFn)(nullptr, thisptr, attribute, value);
#endif
    }

#ifdef _WIN32
    class KeyValuesSystem* keyValuesSystem;
    std::uintptr_t keyValuesAllocEngine;
    std::uintptr_t keyValuesAllocClient;

    std::uintptr_t shouldDrawFogReturnAddress;
#endif

private:
    void(THISCALL_CONV* makePanoramaSymbolFn)(short* symbol, const char* name);

    std::uintptr_t submitReportFunction;
    std::uintptr_t setDynamicAttributeValueFn;
};

inline std::optional<const Memory> memory;
