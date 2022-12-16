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

#include "Utils/ReturnAddress.h"
#include "Utils/SafeAddress.h"
#include "RetSpoofGadgets.h"
#include "Helpers/PatternFinder.h"
#include "Utils/TypeHint.h"

#include "Platform/Macros/CallingConventions.h"

class KeyValues;

namespace csgo
{

struct ClientMode;
class ClientSharedObjectCache;
class CSPlayerInventory;
class EconItem;
class EconItemAttributeDefinition;
class Entity;
class GameEventDescriptor;
class GameEventManager;
class Input;
class MoveData;
class PlantedC4;
class PlayerResource;
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

}

namespace csgo { struct ViewRender; }

namespace csgo::pod
{
    struct Client;
    struct EconItem;
    struct ItemSystem;
    struct MemAlloc;
    struct UiComponentInventory;
}

class Memory {
public:
    Memory(const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder, csgo::pod::Client* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

#if IS_WIN32()
    std::uintptr_t present;
    std::uintptr_t reset;
#endif

    csgo::pod::MemAlloc* memAlloc;
    csgo::ClientMode* clientMode;
    csgo::Input* input;
    csgo::GlobalVars* globalVars;
    csgo::UtlVector<csgo::PlantedC4*>* plantedC4s;
    csgo::UtlMap<short, csgo::PanoramaEventRegistration>* registeredPanoramaEvents;

    std::add_pointer_t<bool CDECL_CONV(csgo::Vector, csgo::Vector, short)> lineGoesThroughSmoke;
    bool(THISCALL_CONV* isOtherEnemy)(std::uintptr_t, std::uintptr_t);
    std::uintptr_t hud;
    int*(THISCALL_CONV* findHudElement)(std::uintptr_t, const char*);
    int(THISCALL_CONV* clearHudWeapon)(int*, int);
    void(THISCALL_CONV* setAbsOrigin)(std::uintptr_t, const csgo::Vector&);
    int* dispatchSound;
    std::uintptr_t traceToExit;
    csgo::ViewRender* viewRender;
    std::uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<void CDECL_CONV(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void CDECL_CONV(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    int(THISCALL_CONV* equipWearable)(csgo::pod::Entity* wearable, csgo::pod::Entity* player);
    int* predictionRandomSeed;
    csgo::MoveData* moveData;
    std::uintptr_t keyValuesFromString;
    KeyValues*(THISCALL_CONV* keyValuesFindKey)(KeyValues* keyValues, const char* keyName, bool create);
    void(THISCALL_CONV* keyValuesSetString)(KeyValues* keyValues, const char* value);
    csgo::WeaponSystem weaponSystem;
    TypeHint<std::uintptr_t, csgo::GameEventDescriptor*(THISCALL_CONV*)(csgo::pod::GameEventManager* thisptr, const char* name, int* cookie)> getEventDescriptor;
    csgo::ActiveChannels* activeChannels;
    csgo::Channel* channels;
    csgo::PlayerResource** playerResource;
    const wchar_t*(THISCALL_CONV* getDecoratedPlayerName)(csgo::PlayerResource* pr, int index, wchar_t* buffer, int buffsize, int flags);
    csgo::pod::Entity** gameRules;
    csgo::InventoryManager inventoryManager;
    csgo::pod::PanoramaMarshallHelper* panoramaMarshallHelper;
    std::add_pointer_t<csgo::pod::EconItemView* CDECL_CONV(std::uint64_t itemID)> findOrCreateEconItemViewForItemID;
    std::uintptr_t createBaseTypeCache;
    TypeHint<std::uintptr_t, void(THISCALL_CONV*)(csgo::pod::UiComponentInventory* thisptr, std::uint64_t itemID, const char* type, const char* value)> setItemSessionPropertyValue;

    short makePanoramaSymbol(const char* name) const noexcept
    {
        short symbol;
        makePanoramaSymbolFn(&symbol, name);
        return symbol;
    }

    [[nodiscard]] csgo::ItemSystem itemSystem() const noexcept
    {
        return csgo::ItemSystem::from(retSpoofGadgets->client, itemSystemFn());
    }

    [[nodiscard]] csgo::MoveHelper moveHelper() const noexcept
    {
        return csgo::MoveHelper::from(retSpoofGadgets->client, moveHelperPtr);
    }

#if IS_WIN32()
    class KeyValuesSystem* keyValuesSystem;
    ReturnAddress keyValuesAllocEngine;
    ReturnAddress keyValuesAllocClient;

    ReturnAddress shouldDrawFogReturnAddress;
#endif

private:
    void(THISCALL_CONV* makePanoramaSymbolFn)(short* symbol, const char* name);
    std::add_pointer_t<csgo::pod::ItemSystem* CDECL_CONV()> itemSystemFn;

    csgo::pod::MoveHelper* moveHelperPtr;
};

inline std::optional<const Memory> memory;
