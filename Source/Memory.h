#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>

#include "CSGO/Constants/DllNames.h"
#include "CSGO/Functions.h"
#include "CSGO/ItemSchema.h"
#include "CSGO/LocalPlayer.h"
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

#include "Platform/DynamicLibrary.h"
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
    template <typename PlatformApi>
    Memory(PlatformApi, const ClientPatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

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

template <typename PlatformApi>
Memory::Memory(PlatformApi, const ClientPatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept
    : plantedC4s{ clientPatternFinder.plantedC4s() },
    registeredPanoramaEvents{ clientPatternFinder.registeredPanoramaEvents() },
    lineGoesThroughSmoke{ retSpoofGadgets.client, clientPatternFinder.lineGoesThroughSmoke() },
    isOtherEnemy{ clientPatternFinder.isOtherEnemy() },
    hud{ clientPatternFinder.hud() },
    findHudElement{ clientPatternFinder.findHudElement() },
    clearHudWeapon{ clientPatternFinder.clearHudWeapon() },
    setAbsOrigin{ clientPatternFinder.setAbsOrigin() },
    traceToExit{ clientPatternFinder.traceToExit() },
    viewRender{ clientPatternFinder.viewRender() },
    drawScreenEffectMaterial{ clientPatternFinder.drawScreenEffectMaterial() },
    equipWearable{ clientPatternFinder.equipWearable() },
    predictionRandomSeed{ clientPatternFinder.predictionRandomSeed() },
    moveData{ clientPatternFinder.moveData() },
    weaponSystem{ retSpoofGadgets.client, clientPatternFinder.weaponSystem() },
    playerResource{ clientPatternFinder.playerResource() },
    getDecoratedPlayerName{ clientPatternFinder.getDecoratedPlayerName() },
    gameRules{ clientPatternFinder.gameRules() },
    inventoryManager{ csgo::InventoryManager::from(retSpoofGadgets.client, clientPatternFinder.inventoryManager()) },
    panoramaMarshallHelper{ clientPatternFinder.panoramaMarshallHelper() },
    findOrCreateEconItemViewForItemID{ retSpoofGadgets.client, clientPatternFinder.findOrCreateEconItemViewForItemID() },
    createBaseTypeCache{ clientPatternFinder.createBaseTypeCache() },
    makePanoramaSymbolFn{ retSpoofGadgets.client, clientPatternFinder.makePanoramaSymbol() },
    itemSystemFn{ clientPatternFinder.getItemSystem() }
{
#if IS_WIN32() || IS_WIN64()
    const DynamicLibrary<PlatformApi> gameOverlayRenderer{ "gameoverlayrenderer.dll" };

    PatternNotFoundHandler patternNotFoundHandler;
    present = PatternFinder{ gameOverlayRenderer.getCodeSection(), patternNotFoundHandler }("FF 15 ? ? ? ? 8B F0 85 FF"_pat).add(2).get();
    reset = PatternFinder{ gameOverlayRenderer.getCodeSection(), patternNotFoundHandler }("C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8"_pat).add(9).get();

    soundMessages = enginePatternFinder("74 3D 8B 0D ? ? ? ? 56"_pat).add(4).deref().add(-4).as<decltype(soundMessages)>();
    splitScreen = enginePatternFinder("79 23 A1"_pat).add(3).deref().as<csgo::SplitScreen*>();
    clientMode = **reinterpret_cast<csgo::ClientMode***>((*reinterpret_cast<uintptr_t**>(clientInterface))[10] + 5);
    input = *reinterpret_cast<csgo::Input**>((*reinterpret_cast<uintptr_t**>(clientInterface))[16] + 1);
    globalVars = **reinterpret_cast<csgo::GlobalVars***>((*reinterpret_cast<uintptr_t**>(clientInterface))[11] + 10);

    const DynamicLibrary<PlatformApi> tier0{ csgo::TIER0_DLL };
    debugMsg = tier0.getFunctionAddress("Msg").as<decltype(debugMsg)>();
    conColorMsg = tier0.getFunctionAddress("?ConColorMsg@@YAXABVColor@@PBDZZ").as<decltype(conColorMsg)>();
    memAlloc = tier0.getFunctionAddress("g_pMemAlloc").deref().as<csgo::MemAllocPOD*>();

    moveHelperPtr = clientPatternFinder.moveHelper();
    getEventDescriptor = enginePatternFinder("E8 ? ? ? ? 8B D8 85 DB 75 27"_pat).add(1).abs().as<csgo::GetEventDescriptor>();
    activeChannels = enginePatternFinder("8B 1D ? ? ? ? 89 5C 24 48"_pat).add(2).deref().as<csgo::ActiveChannels*>();
    channels = enginePatternFinder("81 C2 ? ? ? ? 8B 72 54"_pat).add(2).deref().as<csgo::Channel*>();

    localPlayer.init(clientPatternFinder.localPlayer());

    keyValuesSystem = reinterpret_cast<csgo::KeyValuesSystemPOD * (STDCALL_CONV*)()>(GetProcAddress(GetModuleHandleW(L"vstdlib"), "KeyValuesSystem"))();
    keyValuesAllocEngine = ReturnAddress{ enginePatternFinder("E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C"_pat).add(1).abs().add(0x4A).get() };
    keyValuesAllocClient = clientPatternFinder.keyValuesAlloc();
    shouldDrawFogReturnAddress = clientPatternFinder.shouldDrawFog();
#elif IS_LINUX()
    const DynamicLibrary<PlatformApi> tier0{ csgo::TIER0_DLL };
    debugMsg = tier0.getFunctionAddress("Msg").as<decltype(debugMsg)>();
    conColorMsg = tier0.getFunctionAddress("_Z11ConColorMsgRK5ColorPKcz").as<decltype(conColorMsg)>();

    soundMessages = enginePatternFinder("41 5C 5D E9 ? ? ? ? 8B 48 08"_pat).add(-4).abs().as<decltype(soundMessages)>();
    splitScreen = enginePatternFinder("C6 05 ? ? ? ? ? 48 89 05 ? ? ? ? 0F 84"_pat).add(10).abs().as<csgo::SplitScreen*>();

    globalVars = SafeAddress{ (*reinterpret_cast<std::uintptr_t**>(clientInterface))[11] + 16 }.abs().deref().as<csgo::GlobalVars*>();
    clientMode = SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[10] }.add(12).abs().add(4).abs().deref().as<decltype(clientMode)>();
    input = SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[16] }.add(3).abs().deref().as<csgo::Input*>();

    getEventDescriptor = enginePatternFinder("E8 ? ? ? ? 4D 85 F6 74 09"_pat).add(1).abs().as<csgo::GetEventDescriptor>();
    activeChannels = enginePatternFinder("48 8D 3D ? ? ? ? 48 89 DE E8 ? ? ? ? 8B BD"_pat).add(3).abs().as<csgo::ActiveChannels*>();
    channels = enginePatternFinder("48 8D 3D ? ? ? ? 48 0F BF 10"_pat).add(3).abs().as<csgo::Channel*>();
    // drawScreenEffectMaterial = clientPatternFinder("\x55\x48\x89\xE5\x41\x57\x41\x56\x45\x89\xC6\x41\x55\x41\x54\x53").get();

    localPlayer.init(clientPatternFinder.localPlayer());
#endif
}
