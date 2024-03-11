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

#include "Platform/DynamicLibrary.h"
#include "Platform/Macros/CallingConventions.h"
#include "RetSpoof/FunctionInvoker.h"

#include "BytePatterns/ClientPatternFinder.h"
#include "BytePatterns/EnginePatternFinder.h"

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
    Memory(const ClientPatternFinder& clientPatternFinder, const EnginePatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept;

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

inline Memory::Memory(const ClientPatternFinder& clientPatternFinder, const EnginePatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept
    : soundMessages{ enginePatternFinder.soundMessages() },
    splitScreen{ enginePatternFinder.splitScreen() },
    plantedC4s{ clientPatternFinder.plantedC4s() },
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
    weaponSystem{ csgo::WeaponSystem::from(retSpoofGadgets.client, clientPatternFinder.weaponSystem()) },
    getEventDescriptor{ enginePatternFinder.getEventDescriptor() },
    activeChannels{ enginePatternFinder.activeChannels() },
    channels{ enginePatternFinder.channels() },
    playerResource{ clientPatternFinder.playerResource() },
    getDecoratedPlayerName{ clientPatternFinder.getDecoratedPlayerName() },
    gameRules{ clientPatternFinder.gameRules() },
    inventoryManager{ csgo::InventoryManager::from(retSpoofGadgets.client, clientPatternFinder.inventoryManager()) },
    panoramaMarshallHelper{ clientPatternFinder.panoramaMarshallHelper() },
    findOrCreateEconItemViewForItemID{ retSpoofGadgets.client, clientPatternFinder.findOrCreateEconItemViewForItemID() },
    createBaseTypeCache{ clientPatternFinder.createBaseTypeCache() },
    makePanoramaSymbolFn{ retSpoofGadgets.client, clientPatternFinder.makePanoramaSymbol() },
    itemSystemFn{ clientPatternFinder.getItemSystem() },
    moveHelperPtr{ clientPatternFinder.moveHelper() }
#if IS_WIN32() || IS_WIN64()
    , keyValuesAllocEngine{ enginePatternFinder.keyValuesAlloc() }
    , keyValuesAllocClient{ clientPatternFinder.keyValuesAlloc() }
    , shouldDrawFogReturnAddress{ clientPatternFinder.shouldDrawFog() }
#endif
{
    const DynamicLibrary tier0{ csgo::TIER0_DLL };
    debugMsg = tier0.getFunctionAddress("Msg").template as<decltype(debugMsg)>();

#if IS_WIN32() || IS_WIN64()
    const DynamicLibrary gameOverlayRenderer{ "gameoverlayrenderer.dll" };

    PatternNotFoundHandler patternNotFoundHandler;
    present = PatternFinder{ gameOverlayRenderer.getCodeSection().raw(), patternNotFoundHandler}("8B 4D ? A1 ? ? ? ? 51 FF"_pat).add(4).as<std::uintptr_t>();
    reset = PatternFinder{ gameOverlayRenderer.getCodeSection().raw(), patternNotFoundHandler }("E8 ? ? ? ? A1 ? ? ? ? 57 53"_pat).add(6).as<std::uintptr_t>();

    clientMode = **reinterpret_cast<csgo::ClientMode***>((*reinterpret_cast<uintptr_t**>(clientInterface))[10] + 5);
    input = *reinterpret_cast<csgo::Input**>((*reinterpret_cast<uintptr_t**>(clientInterface))[16] + 1);
    globalVars = **reinterpret_cast<csgo::GlobalVars***>((*reinterpret_cast<uintptr_t**>(clientInterface))[11] + 10);

    conColorMsg = tier0.getFunctionAddress("?ConColorMsg@@YAXABVColor@@PBDZZ").template as<decltype(conColorMsg)>();
    memAlloc = tier0.getFunctionAddress("g_pMemAlloc").deref().template as<csgo::MemAllocPOD*>();

    localPlayer.init(clientPatternFinder.localPlayer());

    keyValuesSystem = reinterpret_cast<csgo::KeyValuesSystemPOD * (STDCALL_CONV*)()>(GetProcAddress(GetModuleHandleW(L"vstdlib"), "KeyValuesSystem"))();
#elif IS_LINUX()
    conColorMsg = tier0.getFunctionAddress("_Z11ConColorMsgRK5ColorPKcz").template as<decltype(conColorMsg)>();

    globalVars = SafeAddress{ (*reinterpret_cast<std::uintptr_t***>(clientInterface))[11] + 2 }.abs().deref().as<csgo::GlobalVars*>();
    clientMode = SafeAddress{ (*reinterpret_cast<uintptr_t***>(clientInterface))[10] }.add(12).abs().add(4).abs().deref().as<decltype(clientMode)>();
    input = SafeAddress{ (*reinterpret_cast<uintptr_t***>(clientInterface))[16] }.add(3).abs().deref().as<csgo::Input*>();

    localPlayer.init(clientPatternFinder.localPlayer());
#endif
}
