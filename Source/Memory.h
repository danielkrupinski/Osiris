#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <type_traits>

#include "SDK/Platform.h"

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
class ViewRenderBeams;
class WeaponSystem;
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

class Memory {
public:
    Memory() noexcept;

#ifdef _WIN32
    std::uintptr_t present;
    std::uintptr_t reset;
#else
    std::uintptr_t pollEvent;
    std::uintptr_t swapWindow;
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
    ViewRenderBeams* viewRenderBeams;
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
    WeaponSystem* weaponSystem;
    std::add_pointer_t<const char** FASTCALL_CONV(const char* playerModelName)> getPlayerViewmodelArmConfigForPlayerModel;
    GameEventDescriptor* (THISCALL_CONV* getEventDescriptor)(GameEventManager* thisptr, const char* name, int* cookie);
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
    std::uintptr_t setStickerToolSlotGetArgAsNumberReturnAddress;
    std::uintptr_t setStickerToolSlotGetArgAsStringReturnAddress;
    std::uintptr_t wearItemStickerGetArgAsNumberReturnAddress;
    std::uintptr_t wearItemStickerGetArgAsStringReturnAddress;
    std::uintptr_t setNameToolStringGetArgAsStringReturnAddress;
    std::uintptr_t clearCustomNameGetArgAsStringReturnAddress;
    std::uintptr_t deleteItemGetArgAsStringReturnAddress;
    std::uintptr_t setStatTrakSwapToolItemsGetArgAsStringReturnAddress1;
    std::uintptr_t setStatTrakSwapToolItemsGetArgAsStringReturnAddress2;
    std::uintptr_t acknowledgeNewItemByItemIDGetArgAsStringReturnAddress;

    std::add_pointer_t<EconItemView* CDECL_CONV(std::uint64_t itemID)> findOrCreateEconItemViewForItemID;
    void*(THISCALL_CONV* getInventoryItemByItemID)(CSPlayerInventory* thisptr, std::uint64_t itemID);
    std::uintptr_t useToolGetArgAsStringReturnAddress;
    std::uintptr_t useToolGetArg2AsStringReturnAddress;
    EconItem*(THISCALL_CONV* getSOCData)(void* itemView);
    void(THISCALL_CONV* setCustomName)(EconItem* thisptr, const char* name);
    SharedObjectTypeCache<EconItem>*(THISCALL_CONV* createBaseTypeCache)(ClientSharedObjectCache<EconItem>* thisptr, int classID);
    void** uiComponentInventory;
    void(THISCALL_CONV* setItemSessionPropertyValue)(void* thisptr, std::uint64_t itemID, const char* type, const char* value);

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
#endif

private:
    void(THISCALL_CONV* makePanoramaSymbolFn)(short* symbol, const char* name);

    std::uintptr_t submitReportFunction;
    std::uintptr_t setDynamicAttributeValueFn;
};

inline std::unique_ptr<const Memory> memory;
