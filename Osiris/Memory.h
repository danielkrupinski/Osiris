#pragma once

#include <memory>
#include <type_traits>

#include "SDK/Platform.h"

class ClientMode;
class Entity;
class GameEventDescriptor;
class GameEventManager;
class Input;
class ItemSystem;
class KeyValues;
class MoveHelper;
class MoveData;
class PlantedC4;
class PlayerResource;
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
struct Trace;
struct Vector;

class Memory {
public:
    Memory() noexcept;

#ifdef _WIN32
    uintptr_t present;
    uintptr_t reset;
#else
    uintptr_t pollEvent;
    uintptr_t swapWindow;
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
    uintptr_t cameraThink;
    std::add_pointer_t<bool __CDECL(Vector, Vector, short)> lineGoesThroughSmoke;
    int(__THISCALL* getSequenceActivity)(void*, int);
    bool(__THISCALL* isOtherEnemy)(Entity*, Entity*);
    uintptr_t hud;
    int*(__THISCALL* findHudElement)(uintptr_t, const char*);
    int(__THISCALL* clearHudWeapon)(int*, int);
    std::add_pointer_t<ItemSystem* __CDECL()> itemSystem;
    void(__THISCALL* setAbsOrigin)(Entity*, const Vector&);
    uintptr_t listLeaves;
    int* dispatchSound;
    uintptr_t traceToExit;
    ViewRender* viewRender;
    ViewRenderBeams* viewRenderBeams;
    uintptr_t drawScreenEffectMaterial;
    uint8_t* fakePrime;
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
    uintptr_t scopeDust;
    uintptr_t scopeArc;
    uintptr_t demoOrHLTV;
    uintptr_t money;
    uintptr_t demoFileEndReached;
    Entity** gameRules;

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

private:
    void(__THISCALL* setOrAddAttributeValueByNameFunction)(std::uintptr_t, const char* attribute);
    void(__THISCALL* makePanoramaSymbolFn)(short* symbol, const char* name);

    std::uintptr_t submitReportFunction;
};

inline std::unique_ptr<const Memory> memory;
