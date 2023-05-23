#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <limits>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "Platform/Macros/IsPlatform.h"
#include "Platform/PlatformApi.h"

#if IS_WIN32()
#include <Windows.h>
#include <Psapi.h>
#elif IS_LINUX()
#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "Memory.h"
#include "CSGO/Constants/DllNames.h"
#include "CSGO/ItemSchema.h"
#include "CSGO/LocalPlayer.h"

#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include "Utils/SafeAddress.h"

Memory::Memory(const ClientPatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept
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

    const auto tier0 = GetModuleHandleW(L"tier0");
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(tier0, "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    memAlloc = *reinterpret_cast<csgo::MemAllocPOD**>(GetProcAddress(tier0, "g_pMemAlloc"));

    moveHelperPtr = clientPatternFinder.moveHelper();
    getEventDescriptor = enginePatternFinder("E8 ? ? ? ? 8B D8 85 DB 75 27"_pat).add(1).abs().as<csgo::GetEventDescriptor>();
    activeChannels = enginePatternFinder("8B 1D ? ? ? ? 89 5C 24 48"_pat).add(2).deref().as<csgo::ActiveChannels*>();
    channels = enginePatternFinder("81 C2 ? ? ? ? 8B 72 54"_pat).add(2).deref().as<csgo::Channel*>();

    localPlayer.init(clientPatternFinder.localPlayer());

    keyValuesSystem = reinterpret_cast<csgo::KeyValuesSystemPOD* (STDCALL_CONV*)()>(GetProcAddress(GetModuleHandleW(L"vstdlib"), "KeyValuesSystem"))();
    keyValuesAllocEngine = ReturnAddress{ enginePatternFinder("E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C"_pat).add(1).abs().add(0x4A).get() };
    keyValuesAllocClient = clientPatternFinder.keyValuesAlloc();
    shouldDrawFogReturnAddress = clientPatternFinder.shouldDrawFog();
#elif IS_LINUX()
    const auto tier0 = dlopen(csgo::TIER0_DLL, RTLD_NOLOAD | RTLD_NOW);
    debugMsg = decltype(debugMsg)(dlsym(tier0, "Msg"));
    conColorMsg = decltype(conColorMsg)(dlsym(tier0, "_Z11ConColorMsgRK5ColorPKcz"));
    dlclose(tier0);

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
