#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>

#include "SDK/EconItemView.h"
#include "SDK/ItemSchema.h"
#include "SDK/MoveHelper.h"
#include "SDK/PanoramaMarshallHelper.h"
#include "SDK/ViewRenderBeams.h"
#include "SDK/WeaponSystem.h"

#include "SafeAddress.h"
#include "RetSpoofGadgets.h"
#include "Helpers/PatternFinder.h"
#include "Utils/TypeHint.h"

#include "Platform/CallingConventions.h"

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
    struct Client;
    struct EconItem;
    struct ItemSystem;
    struct MemAlloc;
}

class Memory {
public:
    Memory(const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder, csgo::pod::Client* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

#if IS_WIN32()
    std::uintptr_t present;
    std::uintptr_t reset;
#endif

    csgo::pod::MemAlloc* memAlloc;
    ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;
    UtlVector<PlantedC4*>* plantedC4s;
    UtlMap<short, PanoramaEventRegistration>* registeredPanoramaEvents;

    std::add_pointer_t<void FASTCALL_CONV(const char*)> loadSky;
    std::add_pointer_t<void FASTCALL_CONV(const char*, const char*)> setClanTag;
    std::uintptr_t cameraThink;
    std::add_pointer_t<bool CDECL_CONV(Vector, Vector, short)> lineGoesThroughSmoke;
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
    TypeHint<std::uintptr_t, GameEventDescriptor*(THISCALL_CONV*)(csgo::pod::GameEventManager* thisptr, const char* name, int* cookie)> getEventDescriptor;
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
    csgo::pod::PanoramaMarshallHelper* panoramaMarshallHelper;
    std::add_pointer_t<csgo::pod::EconItemView* CDECL_CONV(std::uint64_t itemID)> findOrCreateEconItemViewForItemID;
    std::uintptr_t createBaseTypeCache;
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

private:
    void(THISCALL_CONV* makePanoramaSymbolFn)(short* symbol, const char* name);
    std::add_pointer_t<csgo::pod::ItemSystem* CDECL_CONV()> itemSystemFn;

    std::uintptr_t submitReportFunction;
    csgo::pod::MoveHelper* moveHelperPtr;
};

inline std::optional<const Memory> memory;
