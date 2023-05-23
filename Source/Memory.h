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

#include "BytePatterns/ClientPatternFinder.h"

class KeyValues;

namespace csgo
{

struct ClientMode;
struct ClientSharedObjectCache;
struct CSPlayerInventory;
struct EconItem;
struct EconItemAttributeDefinition;
struct Entity;
struct GameEventDescriptor;
struct GameEventManager;
struct Input;
struct MoveData;
struct PlantedC4POD;
struct PlayerResource;
template <typename Key, typename Value>
struct UtlMap;
template <typename T>
struct UtlVector;

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
struct ItemSystemPOD;
struct MemAllocPOD;
struct KeyValuesPOD;
struct ViewRender;
struct HudPOD;

}

class Memory {
public:
    Memory(const ClientPatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

#if IS_WIN32() || IS_WIN64()
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
    csgo::IsOtherEnemy isOtherEnemy;
    csgo::HudPOD* hud;
    csgo::FindHudElement findHudElement;
    csgo::ClearHudWeapon clearHudWeapon;
    csgo::SetAbsOrigin setAbsOrigin;
    std::uintptr_t traceToExit;
    csgo::ViewRender* viewRender;
    std::uintptr_t drawScreenEffectMaterial;
    csgo::DebugMsg debugMsg;
    csgo::ConColorMsg conColorMsg;
    csgo::EquipWearable equipWearable;
    int* predictionRandomSeed;
    csgo::MoveData* moveData;
    csgo::WeaponSystem weaponSystem;
    csgo::GetEventDescriptor getEventDescriptor;
    csgo::ActiveChannels* activeChannels;
    csgo::Channel* channels;
    csgo::PlayerResource** playerResource;
    csgo::GetDecoratedPlayerName getDecoratedPlayerName;
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

#if IS_WIN32() || IS_WIN64()
    csgo::KeyValuesSystemPOD* keyValuesSystem;
    ReturnAddress keyValuesAllocEngine;
    ReturnAddress keyValuesAllocClient;

    ReturnAddress shouldDrawFogReturnAddress;
#endif

private:
    FunctionInvoker<csgo::MakePanoramaSymbol> makePanoramaSymbolFn;
    csgo::GetItemSystem itemSystemFn;

    csgo::MoveHelperPOD* moveHelperPtr;
};
