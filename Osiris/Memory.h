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

    std::add_pointer_t<void __FASTCALL(const char*)> loadSky;
    std::add_pointer_t<void __FASTCALL(const char*, const char*)> setClanTag;
    std::uintptr_t cameraThink;
    std::add_pointer_t<bool __CDECL(Vector, Vector, short)> lineGoesThroughSmoke;
    int(__THISCALL* getSequenceActivity)(void*, int);
    bool(__THISCALL* isOtherEnemy)(Entity*, Entity*);
    std::uintptr_t hud;
    int*(__THISCALL* findHudElement)(std::uintptr_t, const char*);
    int(__THISCALL* clearHudWeapon)(int*, int);
    std::add_pointer_t<ItemSystem* __CDECL()> itemSystem;
    void(__THISCALL* setAbsOrigin)(Entity*, const Vector&);
    std::uintptr_t listLeaves;
    int* dispatchSound;
    std::uintptr_t traceToExit;
    ViewRender* viewRender;
    ViewRenderBeams* viewRenderBeams;
    std::uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<void __CDECL(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void __CDECL(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    float* vignette;
    int(__THISCALL* equipWearable)(void* wearable, void* player);
    int* predictionRandomSeed;
    MoveData* moveData;
    MoveHelper* moveHelper;
    std::uintptr_t keyValuesFromString;
    KeyValues*(__THISCALL* keyValuesFindKey)(KeyValues* keyValues, const char* keyName, bool create);
    void(__THISCALL* keyValuesSetString)(KeyValues* keyValues, const char* value);
    WeaponSystem* weaponSystem;
    std::add_pointer_t<const char** __FASTCALL(const char* playerModelName)> getPlayerViewmodelArmConfigForPlayerModel;
    GameEventDescriptor* (__THISCALL* getEventDescriptor)(GameEventManager* _this, const char* name, int* cookie);
    ActiveChannels* activeChannels;
    Channel* channels;
    PlayerResource** playerResource;
    const wchar_t*(__THISCALL* getDecoratedPlayerName)(PlayerResource* pr, int index, wchar_t* buffer, int buffsize, int flags);
    std::uintptr_t scopeDust;
    std::uintptr_t scopeArc;
    std::uintptr_t demoOrHLTV;
    std::uintptr_t money;
    std::uintptr_t demoFileEndReached;
    Entity** gameRules;
    InventoryManager* inventoryManager;
    std::add_pointer_t<EconItem* __STDCALL()> createEconItemSharedObject;
    bool(__THISCALL* addEconItem)(CSPlayerInventory* _this, EconItem* item, bool updateAckFile, bool writeAckFile, bool checkForNewItems);
    void(__THISCALL* clearInventoryImageRGBA)(void* itemView);
    PanoramaMarshallHelper* panoramaMarshallHelper;
    std::uintptr_t setStickerToolSlotGetArgAsNumberReturnAddress;
    std::uintptr_t setStickerToolSlotGetArgAsStringReturnAddress;
    std::uintptr_t wearItemStickerGetArgAsNumberReturnAddress;
    std::uintptr_t wearItemStickerGetArgAsStringReturnAddress;
    std::uintptr_t setNameToolStringGetArgAsStringReturnAddress;
    std::uintptr_t clearCustomNameGetArgAsStringReturnAddress;

    std::add_pointer_t<void* __CDECL(std::uint64_t itemID)> findOrCreateEconItemViewForItemID;
    void*(__THISCALL* getInventoryItemByItemID)(CSPlayerInventory* _this, std::uint64_t itemID);
    std::uintptr_t useToolStickerGetArgAsStringReturnAddress;
    std::uintptr_t useToolGetArg2AsStringReturnAddress;
    EconItem*(__THISCALL* getSOCData)(void* itemView);
    void(__THISCALL* setCustomName)(EconItem* _this, const char* name);
    SharedObjectTypeCache<EconItem>*(__THISCALL* createBaseTypeCache)(ClientSharedObjectCache<EconItem>* _this, int classID);

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

    void setOrAddAttributeValueByName(std::uintptr_t attributeList, const char* attribute, float value) const noexcept
    {
#ifdef _WIN32
        __asm movd xmm2, value
#else
        asm("movss %0, %%xmm0" : : "m"(value) : "xmm0");
#endif
        setOrAddAttributeValueByNameFunction(attributeList, attribute);
    }

    void setOrAddAttributeValueByName(std::uintptr_t attributeList, const char* attribute, int value) const noexcept
    {
        setOrAddAttributeValueByName(attributeList, attribute, *reinterpret_cast<float*>(&value) /* hack, but CSGO does that */);
    }

    void setDynamicAttributeValue(EconItem* _this, EconItemAttributeDefinition* attribute, void* value) const noexcept
    {
#ifdef _WIN32
        reinterpret_cast<void(__thiscall*)(EconItem*, EconItemAttributeDefinition*, void*)>(setDynamicAttributeValueFn)(_this, attribute, value);
#else
        reinterpret_cast<void(*)(void*, EconItem*, EconItemAttributeDefinition*, void*)>(setDynamicAttributeValueFn)(nullptr, _this, attribute, value);
#endif
    }

private:
    void(__THISCALL* setOrAddAttributeValueByNameFunction)(std::uintptr_t, const char* attribute);
    void(__THISCALL* makePanoramaSymbolFn)(short* symbol, const char* name);

    std::uintptr_t submitReportFunction;
    std::uintptr_t setDynamicAttributeValueFn;
};

inline std::unique_ptr<const Memory> memory;
