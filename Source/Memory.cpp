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

#if IS_WIN32()
#include <Windows.h>
#include <Psapi.h>

#include <Platform/Windows/DynamicLibrary.h>
#include <Platform/Windows/DynamicLibrarySection.h>
#include <Platform/Windows/DynamicLibraryWrapper.h>
#elif IS_LINUX()
#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Platform/Linux/DynamicLibrarySection.h>
#endif

#include "Memory.h"
#include "CSGO/Constants/DllNames.h"
#include "CSGO/ItemSchema.h"
#include "CSGO/LocalPlayer.h"

#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>
#include "Utils/SafeAddress.h"

Memory::Memory(const PatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder, csgo::ClientPOD* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept
#if IS_WIN32()
    : lineGoesThroughSmoke{ retSpoofGadgets.client, clientPatternFinder("E8 ? ? ? ? 8B 4C 24 30 33 D2"_pat).add(1).relativeToAbsolute().get() },
      weaponSystem{ retSpoofGadgets.client, clientPatternFinder("8B 35 ? ? ? ? FF 10 0F B7 C0"_pat).add(2).deref().get() },
      inventoryManager{ csgo::InventoryManager::from(retSpoofGadgets.client, clientPatternFinder("8D 44 24 28 B9 ? ? ? ? 50"_pat).add(5).deref().as<csgo::InventoryManagerPOD*>()) },
      findOrCreateEconItemViewForItemID{ retSpoofGadgets.client, clientPatternFinder("E8 ? ? ? ? 8B CE 83 C4 08"_pat).add(1).relativeToAbsolute().get() },
      makePanoramaSymbolFn{ retSpoofGadgets.client, clientPatternFinder("E8 ? ? ? ? 0F B7 45 0E 8D 4D 0E"_pat).add(1).relativeToAbsolute().get() }
#else
    : lineGoesThroughSmoke{ retSpoofGadgets.client, clientPatternFinder("E8 ? ? ? ? 44 0F B6 4D ? 84 C0"_pat).add(1).relativeToAbsolute().get() },
      weaponSystem{ retSpoofGadgets.client, clientPatternFinder("48 8B 3D ? ? ? ? 48 8B 4D F8"_pat).add(3).relativeToAbsolute().deref().get() },
      inventoryManager{ csgo::InventoryManager::from(retSpoofGadgets.client, clientPatternFinder("48 8D 3D ? ? ? ? E8 ? ? ? ? 48 8D 15 ? ? ? ? C9"_pat).add(3).relativeToAbsolute().as<csgo::InventoryManagerPOD*>()) },
      findOrCreateEconItemViewForItemID{ retSpoofGadgets.client, clientPatternFinder("E8 ? ? ? ? 49 89 C5 4D 85 E4"_pat).add(1).relativeToAbsolute().get() },
      makePanoramaSymbolFn{ retSpoofGadgets.client, clientPatternFinder("E8 ? ? ? ? 0F B7 45 A0 31 F6"_pat).add(1).relativeToAbsolute().get() }
#endif
{
#if IS_WIN32()
    const windows_platform::DynamicLibrary gameOverlayRenderer{ windows_platform::DynamicLibraryWrapper{}, "gameoverlayrenderer" };

    PatternNotFoundHandler patternNotFoundHandler;
    present = PatternFinder{ getCodeSection(gameOverlayRenderer.getView()), patternNotFoundHandler }("FF 15 ? ? ? ? 8B F0 85 FF"_pat).add(2).get();
    reset = PatternFinder{ getCodeSection(gameOverlayRenderer.getView()), patternNotFoundHandler }("C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8"_pat).add(9).get();

    soundMessages = enginePatternFinder("74 3D 8B 0D ? ? ? ? 56"_pat).add(4).deref().add(-4).as<decltype(soundMessages)>();
    splitScreen = enginePatternFinder("79 23 A1"_pat).add(3).deref().as<csgo::SplitScreen*>();
    clientMode = **reinterpret_cast<csgo::ClientMode***>((*reinterpret_cast<uintptr_t**>(clientInterface))[10] + 5);
    input = *reinterpret_cast<csgo::Input**>((*reinterpret_cast<uintptr_t**>(clientInterface))[16] + 1);
    globalVars = **reinterpret_cast<csgo::GlobalVars***>((*reinterpret_cast<uintptr_t**>(clientInterface))[11] + 10);
    isOtherEnemy = clientPatternFinder("8B CE E8 ? ? ? ? 02 C0"_pat).add(3).relativeToAbsolute().as<decltype(isOtherEnemy)>();
    auto temp = clientPatternFinder("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"_pat).add(1);
    hud = SafeAddress{ temp }.deref().get();
    findHudElement = temp.add(5).relativeToAbsolute().as<decltype(findHudElement)>();
    clearHudWeapon = clientPatternFinder("E8 ? ? ? ? 8B F0 C6 44 24 ? ? C6 44 24"_pat).add(1).relativeToAbsolute().as<decltype(clearHudWeapon)>();
    itemSystemFn = clientPatternFinder("E8 ? ? ? ? 0F B7 0F"_pat).add(1).relativeToAbsolute().as<decltype(itemSystemFn)>();
    setAbsOrigin = clientPatternFinder("E8 ? ? ? ? EB 19 8B 07"_pat).add(1).relativeToAbsolute().as<decltype(setAbsOrigin)>();
    traceToExit = clientPatternFinder("55 8B EC 83 EC 4C F3 0F 10 75"_pat).get();
    viewRender = clientPatternFinder("8B 0D ? ? ? ? FF 75 0C 8B 45 08"_pat).add(2).deref<2>().as<csgo::ViewRender*>();
    drawScreenEffectMaterial = clientPatternFinder("E8 ? ? ? ? 83 C4 0C 8D 4D F8"_pat).add(1).relativeToAbsolute().get();
    const auto tier0 = GetModuleHandleW(L"tier0");
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(tier0, "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    memAlloc = *reinterpret_cast<csgo::MemAllocPOD**>(GetProcAddress(tier0, "g_pMemAlloc"));
    equipWearable = clientPatternFinder("55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9"_pat).as<decltype(equipWearable)>();
    predictionRandomSeed = clientPatternFinder("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04"_pat).add(2).deref().as<int*>();
    moveData = clientPatternFinder("A1 ? ? ? ? F3 0F 59 CD"_pat).add(1).deref<2>().as<csgo::MoveData*>();
    moveHelperPtr = clientPatternFinder("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01"_pat).add(2).deref<2>().as<csgo::MoveHelperPOD*>();
    getEventDescriptor = enginePatternFinder("E8 ? ? ? ? 8B D8 85 DB 75 27"_pat).add(1).relativeToAbsolute().as<csgo::GetEventDescriptor>();
    activeChannels = enginePatternFinder("8B 1D ? ? ? ? 89 5C 24 48"_pat).add(2).deref().as<csgo::ActiveChannels*>();
    channels = enginePatternFinder("81 C2 ? ? ? ? 8B 72 54"_pat).add(2).deref().as<csgo::Channel*>();
    playerResource = clientPatternFinder("74 30 8B 35 ? ? ? ? 85 F6"_pat).add(4).deref().as<csgo::PlayerResource**>();
    getDecoratedPlayerName = clientPatternFinder("E8 ? ? ? ? 66 83 3E"_pat).add(1).relativeToAbsolute().as<decltype(getDecoratedPlayerName)>();
    plantedC4s = clientPatternFinder("7E 2C 8B 15"_pat).add(4).deref().as<decltype(plantedC4s)>();
    gameRules = clientPatternFinder("8B EC 8B 0D ? ? ? ? 85 C9 74 07"_pat).add(4).deref().as<csgo::EntityPOD**>();
    registeredPanoramaEvents = clientPatternFinder("E8 ? ? ? ? A1 ? ? ? ? A8 01 75 21"_pat).add(6).deref().add(-36).as<decltype(registeredPanoramaEvents)>();
    panoramaMarshallHelper = clientPatternFinder("68 ? ? ? ? 8B C8 E8 ? ? ? ? 8D 4D F4 FF 15 ? ? ? ? 8B CF FF 15 ? ? ? ? 5F 5E 8B E5 5D C3"_pat).add(1).deref().as<decltype(panoramaMarshallHelper)>();

    createBaseTypeCache = clientPatternFinder("E8 ? ? ? ? 8D 4D 0F"_pat).add(1).relativeToAbsolute().as<csgo::CreateBaseTypeCache>();

    localPlayer.init(clientPatternFinder("A1 ? ? ? ? 89 45 BC 85 C0"_pat).add(1).deref().as<csgo::EntityPOD**>());

    keyValuesSystem = reinterpret_cast<csgo::KeyValuesSystemPOD* (STDCALL_CONV*)()>(GetProcAddress(GetModuleHandleW(L"vstdlib"), "KeyValuesSystem"))();
    keyValuesAllocEngine = ReturnAddress{ enginePatternFinder("E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C"_pat).add(1).relativeToAbsolute().add(0x4A).get() };
    keyValuesAllocClient = ReturnAddress{ clientPatternFinder("E8 ? ? ? ? 83 C4 08 84 C0 75 10"_pat).add(1).relativeToAbsolute().add(0x3E).get() };

    shouldDrawFogReturnAddress = ReturnAddress{ clientPatternFinder("E8 ? ? ? ? 8B 0D ? ? ? ? 0F B6 D0"_pat).add(1).relativeToAbsolute().add(82).get() };
#else
    const auto tier0 = dlopen(csgo::TIER0_DLL, RTLD_NOLOAD | RTLD_NOW);
    debugMsg = decltype(debugMsg)(dlsym(tier0, "Msg"));
    conColorMsg = decltype(conColorMsg)(dlsym(tier0, "_Z11ConColorMsgRK5ColorPKcz"));
    dlclose(tier0);

    soundMessages = enginePatternFinder("41 5C 5D E9 ? ? ? ? 8B 48 08"_pat).add(-4).relativeToAbsolute().as<decltype(soundMessages)>();
    splitScreen = enginePatternFinder("C6 05 ? ? ? ? ? 48 89 05 ? ? ? ? 0F 84"_pat).add(10).relativeToAbsolute().as<csgo::SplitScreen*>();

    globalVars = SafeAddress{ (*reinterpret_cast<std::uintptr_t**>(clientInterface))[11] + 16 }.relativeToAbsolute().deref().as<csgo::GlobalVars*>();
    itemSystemFn = clientPatternFinder("E8 ? ? ? ? 44 39 78 44"_pat).add(1).relativeToAbsolute().as<decltype(itemSystemFn)>();

    isOtherEnemy = clientPatternFinder("E8 ? ? ? ? 84 C0 75 94"_pat).add(1).relativeToAbsolute().as<decltype(isOtherEnemy)>();
    getDecoratedPlayerName = clientPatternFinder("E8 ? ? ? ? 41 8B 17 85 D2"_pat).add(1).relativeToAbsolute().as<decltype(getDecoratedPlayerName)>();

    hud = clientPatternFinder("48 8D 3D ? ? ? ? 48 83 EC 10 E8 ? ? ? ? 41 8B 5C 24"_pat).add(3).relativeToAbsolute().get();
    findHudElement = clientPatternFinder("E8 ? ? ? ? 48 8D 50 E0"_pat).add(1).relativeToAbsolute().as<decltype(findHudElement)>();

    clientMode = SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[10] }.add(12).relativeToAbsolute().add(4).relativeToAbsolute().deref().as<decltype(clientMode)>();
    input = SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[16] }.add(3).relativeToAbsolute().deref().as<csgo::Input*>();
    playerResource = clientPatternFinder("48 8B 15 ? ? ? ? 48 63 C3"_pat).add(3).relativeToAbsolute().as<csgo::PlayerResource**>();

    getEventDescriptor = enginePatternFinder("E8 ? ? ? ? 4D 85 F6 74 09"_pat).add(1).relativeToAbsolute().as<csgo::GetEventDescriptor>();
    activeChannels = enginePatternFinder("48 8D 3D ? ? ? ? 48 89 DE E8 ? ? ? ? 8B BD"_pat).add(3).relativeToAbsolute().as<csgo::ActiveChannels*>();
    channels = enginePatternFinder("48 8D 3D ? ? ? ? 48 0F BF 10"_pat).add(3).relativeToAbsolute().as<csgo::Channel*>();
    // drawScreenEffectMaterial = clientPatternFinder("\x55\x48\x89\xE5\x41\x57\x41\x56\x45\x89\xC6\x41\x55\x41\x54\x53").get();
    viewRender = clientPatternFinder("0F 85 ? ? ? ? 48 8D 05 ? ? ? ? 45 89 F8"_pat).add(9).relativeToAbsolute().deref().as<csgo::ViewRender*>();
    clearHudWeapon = clientPatternFinder("E8 ? ? ? ? C6 45 AE 01"_pat).add(1).relativeToAbsolute().as<decltype(clearHudWeapon)>();
    equipWearable = clientPatternFinder("55 48 8D 15 ? ? ? ? 48 89 E5 41 56 41 55 41 54 49 89 F4 53 48 8B 07"_pat).as<decltype(equipWearable)>();
    setAbsOrigin = clientPatternFinder("E8 ? ? ? ? 4D 63 B5"_pat).add(1).relativeToAbsolute().as<decltype(setAbsOrigin)>();
    plantedC4s = clientPatternFinder("48 8D 3D ? ? ? ? 42 C6 44 28"_pat).add(3).relativeToAbsolute().as<decltype(plantedC4s)>();
    gameRules = clientPatternFinder("48 8D 1D ? ? ? ? 48 8B 3B 48 85 FF 74 06"_pat).add(3).relativeToAbsolute().as<csgo::EntityPOD**>();
    predictionRandomSeed = clientPatternFinder("41 8D 56 FF 31 C9"_pat).add(-14).relativeToAbsolute().as<int*>();
    registeredPanoramaEvents = clientPatternFinder("E8 ? ? ? ? 44 0F B7 9D"_pat).add(1).relativeToAbsolute().add(12).relativeToAbsolute().as<decltype(registeredPanoramaEvents)>();
    moveData = clientPatternFinder("48 8D 0D ? ? ? ? 48 89 DE 4C 89 FF"_pat).add(3).relativeToAbsolute().deref().as<csgo::MoveData*>();
    moveHelperPtr = clientPatternFinder("48 8D 05 ? ? ? ? 48 8B 38 48 8B 07 FF 50 58 41 0F B7 F7"_pat).add(3).relativeToAbsolute().deref().as<csgo::MoveHelperPOD*>();

    panoramaMarshallHelper = clientPatternFinder("48 89 05 ? ? ? ? 48 C7 05 ? ? ? ? ? ? ? ? 48 8D 15"_pat).add(3).relativeToAbsolute().as<decltype(panoramaMarshallHelper)>();
    createBaseTypeCache = clientPatternFinder("E8 ? ? ? ? 48 89 DE 5B 48 8B 10"_pat).add(1).relativeToAbsolute().as<csgo::CreateBaseTypeCache>();

    localPlayer.init(clientPatternFinder("83 FF FF 48 8B 05"_pat).add(6).relativeToAbsolute().as<csgo::EntityPOD**>());
#endif
}
