#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>

#include "CSGO/Functions.h"
#include "CSGO/ItemSchema.h"
#include "CSGO/MoveHelper.h"
#include "CSGO/SoundInfo.h"
#include "CSGO/SplitScreen.h"
#include "CSGO/UtlRbTree.h"
#include "CSGO/WeaponSystem.h"

#include "Utils/ReturnAddress.h"
#include "Utils/SafeAddress.h"
#include "RetSpoof/RetSpoofGadgets.h"
#include "MemorySearch/PatternFinder.h"
#include "Utils/TypeHint.h"

#include "Platform/Macros/CallingConventions.h"
#include "RetSpoof/FunctionInvoker.h"

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
struct PlantedC4POD;
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
struct EconItemPOD;
struct UiComponentInventoryPOD;
struct ClientPOD;
struct PanoramaMarshallHelperPOD;
struct KeyValuesSystemPOD;

}

namespace csgo { struct ViewRender; }

namespace csgo
{
    struct ItemSystemPOD;
    struct MemAllocPOD;
    struct KeyValuesPOD;
}

class Memory {
public:
    Memory(const PatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

#if IS_WIN32()
    std::uintptr_t present;
    std::uintptr_t reset;
#endif

    csgo::UtlRbTree<csgo::SoundInfo, int>* soundMessages;
    csgo::SplitScreen* splitScreen;
    csgo::MemAllocPOD* memAlloc;
    csgo::ClientMode* clientMode;
    csgo::Input* input;
    csgo::GlobalVars* globalVars;
    csgo::UtlVector<csgo::PlantedC4POD*>* plantedC4s;
    csgo::UtlMap<short, csgo::PanoramaEventRegistration>* registeredPanoramaEvents;

    FunctionInvoker<csgo::LineGoesThroughSmoke> lineGoesThroughSmoke;
    bool(THISCALL_CONV* isOtherEnemy)(std::uintptr_t, std::uintptr_t);
    std::uintptr_t hud;
    int*(THISCALL_CONV* findHudElement)(std::uintptr_t, const char*);
    int(THISCALL_CONV* clearHudWeapon)(int*, int);
    void(THISCALL_CONV* setAbsOrigin)(std::uintptr_t, const csgo::Vector&);
    std::uintptr_t traceToExit;
    csgo::ViewRender* viewRender;
    std::uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<void CDECL_CONV(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void CDECL_CONV(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    int(THISCALL_CONV* equipWearable)(csgo::EntityPOD* wearable, csgo::EntityPOD* player);
    int* predictionRandomSeed;
    csgo::MoveData* moveData;
    csgo::WeaponSystem weaponSystem;
    csgo::GetEventDescriptor getEventDescriptor;
    csgo::ActiveChannels* activeChannels;
    csgo::Channel* channels;
    csgo::PlayerResource** playerResource;
    const wchar_t*(THISCALL_CONV* getDecoratedPlayerName)(csgo::PlayerResource* pr, int index, wchar_t* buffer, int buffsize, int flags);
    csgo::EntityPOD** gameRules;
    csgo::InventoryManager inventoryManager;
    csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper;
    FunctionInvoker<csgo::FindOrCreateEconItemViewForItemID> findOrCreateEconItemViewForItemID;
    csgo::CreateBaseTypeCache createBaseTypeCache;

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
    csgo::KeyValuesSystemPOD* keyValuesSystem;
    ReturnAddress keyValuesAllocEngine;
    ReturnAddress keyValuesAllocClient;

    ReturnAddress shouldDrawFogReturnAddress;
#endif

private:
    FunctionInvoker<csgo::MakePanoramaSymbol> makePanoramaSymbolFn;
    std::add_pointer_t<csgo::ItemSystemPOD* CDECL_CONV()> itemSystemFn;

    csgo::MoveHelperPOD* moveHelperPtr;
};
