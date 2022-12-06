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

#include "Platform/IsPlatform.h"

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
#include "SDK/Constants/DllNames.h"
#include "SDK/ItemSchema.h"
#include "SDK/LocalPlayer.h"

#include "Utils/PatternFinder.h"
#include "Utils/SafeAddress.h"

Memory::Memory(const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder, csgo::pod::Client* clientInterface, const RetSpoofGadgets& retSpoofGadgets) noexcept
#if IS_WIN32()
    : viewRenderBeams{ retSpoofGadgets.client, clientPatternFinder("\xB9????\x0F\x11\x44\x24?\xC7\x44\x24?????\xF3\x0F\x10\x84\x24").add(1).deref().get() },
      weaponSystem{ retSpoofGadgets.client, clientPatternFinder("\x8B\x35????\xFF\x10\x0F\xB7\xC0").add(2).deref().get() },
      inventoryManager{ InventoryManager::from(retSpoofGadgets.client, clientPatternFinder("\x8D\x44\x24\x28\xB9????\x50").add(5).deref().as<csgo::pod::InventoryManager*>()) }
#else
    : viewRenderBeams{ retSpoofGadgets.client, clientPatternFinder("\x4C\x89\xF6\x4C\x8B\x25????\x48\x8D\x05").add(6).relativeToAbsolute().deref<2>().get() },
      weaponSystem{ retSpoofGadgets.client, clientPatternFinder("\x48\x8B\x58\x10\x48\x8B\x07\xFF\x10").add(12).relativeToAbsolute().deref().get() },
      inventoryManager{ InventoryManager::from(retSpoofGadgets.client, clientPatternFinder("\x48\x8D\x35????\x48\x8D\x3D????\xE9????\x90\x90\x90\x55").add(3).relativeToAbsolute().as<csgo::pod::InventoryManager*>()) }
#endif
{
#if IS_WIN32()
    const windows_platform::DynamicLibrary gameOverlayRenderer{ windows_platform::DynamicLibraryWrapper{}, "gameoverlayrenderer" };

    present = helpers::PatternFinder{ getCodeSection(gameOverlayRenderer.getView()) }("\xFF\x15????\x8B\xF0\x85\xFF").add(2).get();
    reset = helpers::PatternFinder{ getCodeSection(gameOverlayRenderer.getView()) }("\xC7\x45?????\xFF\x15????\x8B\xD8").add(9).get();

    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(clientInterface))[10] + 5);
    input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(clientInterface))[16] + 1);
    globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(clientInterface))[11] + 10);
    glowObjectManager = clientPatternFinder("\x0F\x11\x05????\x83\xC8\x01").add(3).deref().as<GlowObjectManager*>();
    setClanTag = enginePatternFinder("\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15").as<decltype(setClanTag)>();
    lineGoesThroughSmoke = clientPatternFinder("\xE8????\x8B\x4C\x24\x30\x33\xD2").add(1).relativeToAbsolute().as<decltype(lineGoesThroughSmoke)>();
    isOtherEnemy = clientPatternFinder("\x8B\xCE\xE8????\x02\xC0").add(3).relativeToAbsolute().as<decltype(isOtherEnemy)>();
    auto temp = clientPatternFinder("\xB9????\xE8????\x8B\x5D\x08").add(1);
    hud = SafeAddress{ temp }.deref().get();
    findHudElement = temp.add(5).relativeToAbsolute().as<decltype(findHudElement)>();
    clearHudWeapon = clientPatternFinder("\xE8????\x8B\xF0\xC6\x44\x24??\xC6\x44\x24").add(1).relativeToAbsolute().as<decltype(clearHudWeapon)>();
    itemSystemFn = clientPatternFinder("\xE8????\x0F\xB7\x0F").add(1).relativeToAbsolute().as<decltype(itemSystemFn)>();
    setAbsOrigin = clientPatternFinder("\xE8????\xEB\x19\x8B\x07").add(1).relativeToAbsolute().as<decltype(setAbsOrigin)>();
    insertIntoTree = ReturnAddress{ clientPatternFinder("\x56\x52\xFF\x50\x18").add(5).get() };
    dispatchSound = enginePatternFinder("\x74\x0B\xE8????\x8B\x3D").add(3).as<int*>();
    traceToExit = clientPatternFinder("\x55\x8B\xEC\x83\xEC\x4C\xF3\x0F\x10\x75").get();
    viewRender = clientPatternFinder("\x8B\x0D????\xFF\x75\x0C\x8B\x45\x08").add(2).deref<2>().as<ViewRender*>();
    drawScreenEffectMaterial = clientPatternFinder("\xE8????\x83\xC4\x0C\x8D\x4D\xF8").add(1).relativeToAbsolute().get();
    submitReportFunction = clientPatternFinder("\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x28\x8B\x4D\x08").get();
    const auto tier0 = GetModuleHandleW(L"tier0");
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(tier0, "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    memAlloc = *reinterpret_cast<csgo::pod::MemAlloc**>(GetProcAddress(tier0, "g_pMemAlloc"));
    equipWearable = clientPatternFinder("\x55\x8B\xEC\x83\xEC\x10\x53\x8B\x5D\x08\x57\x8B\xF9").as<decltype(equipWearable)>();
    predictionRandomSeed = clientPatternFinder("\x8B\x0D????\xBA????\xE8????\x83\xC4\x04").add(2).deref().as<int*>();
    moveData = clientPatternFinder("\xA1????\xF3\x0F\x59\xCD").add(1).deref<2>().as<MoveData*>();
    moveHelperPtr = clientPatternFinder("\x8B\x0D????\x8B\x45?\x51\x8B\xD4\x89\x02\x8B\x01").add(2).deref<2>().as<csgo::pod::MoveHelper*>();
    keyValuesFromString = clientPatternFinder("\xE8????\x83\xC4\x04\x89\x45\xD8").add(1).relativeToAbsolute().get();
    keyValuesFindKey = clientPatternFinder("\xE8????\xF7\x45").add(1).relativeToAbsolute().as<decltype(keyValuesFindKey)>();
    keyValuesSetString = clientPatternFinder("\xE8????\x89\x77\x38").add(1).relativeToAbsolute().as<decltype(keyValuesSetString)>();
    getEventDescriptor = enginePatternFinder("\xE8????\x8B\xD8\x85\xDB\x75\x27").add(1).relativeToAbsolute().get();
    activeChannels = enginePatternFinder("\x8B\x1D????\x89\x5C\x24\x48").add(2).deref().as<ActiveChannels*>();
    channels = enginePatternFinder("\x81\xC2????\x8B\x72\x54").add(2).deref().as<Channel*>();
    playerResource = clientPatternFinder("\x74\x30\x8B\x35????\x85\xF6").add(4).deref().as<PlayerResource**>();
    getDecoratedPlayerName = clientPatternFinder("\xE8????\x66\x83\x3E").add(1).relativeToAbsolute().as<decltype(getDecoratedPlayerName)>();
    demoOrHLTV = ReturnAddress{ clientPatternFinder("\x84\xC0\x75\x09\x38\x05").get() };
    money = clientPatternFinder("\x84\xC0\x75\x0C\x5B").get();
    demoFileEndReached = ReturnAddress{ clientPatternFinder("\x8B\xC8\x85\xC9\x74\x1F\x80\x79\x10").get() };
    plantedC4s = clientPatternFinder("\x7E\x2C\x8B\x15").add(4).deref().as<decltype(plantedC4s)>();
    gameRules = clientPatternFinder("\x8B\xEC\x8B\x0D????\x85\xC9\x74\x07").add(4).deref().as<csgo::pod::Entity**>();
    registeredPanoramaEvents = clientPatternFinder("\xE8????\xA1????\xA8\x01\x75\x21").add(6).deref().add(-36).as<decltype(registeredPanoramaEvents)>();
    makePanoramaSymbolFn = clientPatternFinder("\xE8????\x0F\xB7\x45\x0E\x8D\x4D\x0E").add(1).relativeToAbsolute().as<decltype(makePanoramaSymbolFn)>();
    createEconItemSharedObject = clientPatternFinder("\x55\x8B\xEC\x83\xEC\x1C\x8D\x45\xE4\xC7\x45").add(20).deref().as<decltype(createEconItemSharedObject)>();
    panoramaMarshallHelper = clientPatternFinder("\x68????\x8B\xC8\xE8????\x8D\x4D\xF4\xFF\x15????\x8B\xCF\xFF\x15????\x5F\x5E\x8B\xE5\x5D\xC3").add(1).deref().as<decltype(panoramaMarshallHelper)>();

    findOrCreateEconItemViewForItemID = clientPatternFinder("\xE8????\x8B\xCE\x83\xC4\x08").add(1).relativeToAbsolute().as<decltype(findOrCreateEconItemViewForItemID)>();
    createBaseTypeCache = clientPatternFinder("\xE8????\x8D\x4D\x0F").add(1).relativeToAbsolute().get();
    uiComponentInventory = clientPatternFinder("\xC6\x44\x24??\x83\x3D").add(7).deref().as<csgo::pod::UiComponentInventory**>();
    setItemSessionPropertyValue = clientPatternFinder("\xE8????\x8B\x4C\x24\x2C\x46").add(1).relativeToAbsolute().get();

    localPlayer.init(clientPatternFinder("\xA1????\x89\x45\xBC\x85\xC0").add(1).deref().as<csgo::pod::Entity**>());

    keyValuesSystem = reinterpret_cast<KeyValuesSystem* (STDCALL_CONV*)()>(GetProcAddress(GetModuleHandleW(L"vstdlib"), "KeyValuesSystem"))();
    keyValuesAllocEngine = ReturnAddress{ enginePatternFinder("\xE8????\x83\xC4\x08\x84\xC0\x75\x10\xFF\x75\x0C").add(1).relativeToAbsolute().add(0x4A).get() };
    keyValuesAllocClient = ReturnAddress{ clientPatternFinder("\xE8????\x83\xC4\x08\x84\xC0\x75\x10").add(1).relativeToAbsolute().add(0x3E).get() };

    shouldDrawFogReturnAddress = ReturnAddress{ clientPatternFinder("\xE8????\x8B\x0D????\x0F\xB6\xD0").add(1).relativeToAbsolute().add(82).get() };
#else
    const auto tier0 = dlopen(csgo::TIER0_DLL, RTLD_NOLOAD | RTLD_NOW);
    debugMsg = decltype(debugMsg)(dlsym(tier0, "Msg"));
    conColorMsg = decltype(conColorMsg)(dlsym(tier0, "_Z11ConColorMsgRK5ColorPKcz"));
    dlclose(tier0);

    globalVars = SafeAddress{ (*reinterpret_cast<std::uintptr_t**>(clientInterface))[11] + 16 }.relativeToAbsolute().deref().as<GlobalVars*>();
    itemSystemFn = clientPatternFinder("\xE8????\x4D\x63\xEC").add(1).relativeToAbsolute().as<decltype(itemSystemFn)>();

    isOtherEnemy = clientPatternFinder("\xE8????\x84\xC0\x44\x89\xE2").add(1).relativeToAbsolute().as<decltype(isOtherEnemy)>();
    lineGoesThroughSmoke = clientPatternFinder("\x55\x48\x89\xE5\x41\x56\x41\x55\x41\x54\x53\x48\x83\xEC\x30\x66\x0F\xD6\x45\xD0").as<decltype(lineGoesThroughSmoke)>();
    getDecoratedPlayerName = clientPatternFinder("\xE8????\x8B\x33\x4C\x89\xF7").add(1).relativeToAbsolute().as<decltype(getDecoratedPlayerName)>();

    hud = clientPatternFinder("\x53\x48\x8D\x3D????\x48\x83\xEC\x10\xE8").add(4).relativeToAbsolute().get();
    findHudElement = clientPatternFinder("\xE8????\x48\x8D\x50\xE0").add(1).relativeToAbsolute().as<decltype(findHudElement)>();

    submitReportFunction = clientPatternFinder("\x55\x48\x89\xF7\x48\x89\xE5\x41\x57\x41\x56\x41\x55\x41\x54\x53\x48\x89\xD3\x48\x83\xEC\x58").get();
    clientMode = SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[10] }.add(12).relativeToAbsolute().add(4).relativeToAbsolute().deref().as<decltype(clientMode)>();
    input = SafeAddress{ (*reinterpret_cast<uintptr_t**>(clientInterface))[16] }.add(3).relativeToAbsolute().deref<2>().as<Input*>();
    playerResource = clientPatternFinder("\x74\x38\x48\x8B\x3D????\x89\xDE").add(5).relativeToAbsolute().as<PlayerResource**>();

    glowObjectManager = clientPatternFinder("\xE8????\x4C\x89\xE7\x8B\x70\x20").add(1).relativeToAbsolute().add(12).relativeToAbsolute().as<decltype(glowObjectManager)>();
    setClanTag = enginePatternFinder("\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0").add(1).relativeToAbsolute().as<decltype(setClanTag)>();
    getEventDescriptor = enginePatternFinder("\xE8????\x48\x85\xC0\x74\x62").add(1).relativeToAbsolute().get();
    activeChannels = enginePatternFinder("\x48\x8D\x3D????\x4C\x89\xE6\xE8????\x8B\xBD").add(3).relativeToAbsolute().as<ActiveChannels*>();
    channels = enginePatternFinder("\x4C\x8D\x35????\x49\x83\xC4\x04").add(3).relativeToAbsolute().as<Channel*>();
    keyValuesFromString = clientPatternFinder("\xE8????\x48\x89\xDF\x48\x89\x45\xE0").add(1).relativeToAbsolute().get();
    keyValuesFindKey = clientPatternFinder("\xE8????\x48\x85\xC0\x75\x24").add(1).relativeToAbsolute().as<decltype(keyValuesFindKey)>();
    keyValuesSetString = clientPatternFinder("\xE8????\x4C\x89\xE6\x4C\x89\xFF\xE8????\x48\x8B\x03").add(1).relativeToAbsolute().as<decltype(keyValuesSetString)>();
    // drawScreenEffectMaterial = clientPatternFinder("\x55\x48\x89\xE5\x41\x57\x41\x56\x45\x89\xC6\x41\x55\x41\x54\x53").get();
    viewRender = clientPatternFinder("\x0F\x85????\x48\x8B\x05????\x45\x89\xF8").add(9).relativeToAbsolute().deref<2>().as<ViewRender*>();
    demoOrHLTV = ReturnAddress{ clientPatternFinder("\x0F\xB6\x10\x89\xD0").add(-16).get() };
    money = clientPatternFinder("\x84\xC0\x75\x9E\xB8????\xEB\xB9").get();
    demoFileEndReached = ReturnAddress{ clientPatternFinder("\x48\x85\xC0\x0F\x84????\x80\x78\x10?\x74\x7F").get() };
    clearHudWeapon = clientPatternFinder("\xE8????\xC6\x45\xB8\x01").add(1).relativeToAbsolute().as<decltype(clearHudWeapon)>();
    equipWearable = clientPatternFinder("\x55\x48\x89\xE5\x41\x56\x41\x55\x41\x54\x49\x89\xF4\x53\x48\x89\xFB\x48\x83\xEC\x10\x48\x8B\x07").as<decltype(equipWearable)>();
    setAbsOrigin = clientPatternFinder("\xE8????\x49\x8B\x07\x31\xF6").add(1).relativeToAbsolute().as<decltype(setAbsOrigin)>();
    plantedC4s = clientPatternFinder("\x48\x8D\x3D????\x42\xC6\x44\x28").add(3).relativeToAbsolute().as<decltype(plantedC4s)>();
    gameRules = clientPatternFinder("\x48\x8B\x1D????\x48\x8B\x3B\x48\x85\xFF\x74\x06").add(3).relativeToAbsolute().deref().as<csgo::pod::Entity**>();
    dispatchSound = enginePatternFinder("\x74\x10\xE8????\x48\x8B\x35").add(3).as<int*>();
    predictionRandomSeed = clientPatternFinder("\x41\x8D\x56\xFF\x31\xC9").add(-14).relativeToAbsolute().deref().as<int*>();
    registeredPanoramaEvents = clientPatternFinder("\xE8????\x8B\x50\x10\x49\x89\xC6").add(1).relativeToAbsolute().add(12).relativeToAbsolute().as<decltype(registeredPanoramaEvents)>();
    makePanoramaSymbolFn = clientPatternFinder("\xE8????\x0F\xB7\x45\xA0\x31\xF6").add(1).relativeToAbsolute().as<decltype(makePanoramaSymbolFn)>();
    moveData = clientPatternFinder("\x4C\x8B\x2D????\x0F\xB6\x93").add(3).relativeToAbsolute().deref<2>().as<MoveData*>();
    moveHelperPtr = clientPatternFinder("\x48\x8B\x05????\x44\x89\x85????\x48\x8B\x38").add(3).relativeToAbsolute().deref<2>().as<csgo::pod::MoveHelper*>();

    createEconItemSharedObject = clientPatternFinder("\x55\x48\x8D\x05????\x31\xD2\x4C\x8D\x0D").add(50).relativeToAbsolute().as<decltype(createEconItemSharedObject)>();
    panoramaMarshallHelper = clientPatternFinder("\xF3\x0F\x11\x05????\x48\x89\x05????\x48\xC7\x05????????\xC7\x05").add(11).relativeToAbsolute().as<decltype(panoramaMarshallHelper)>();
    findOrCreateEconItemViewForItemID = clientPatternFinder("\xE8????\x4C\x89\xEF\x48\x89\x45\xC8").add(1).relativeToAbsolute().as<decltype(findOrCreateEconItemViewForItemID)>();
    createBaseTypeCache = clientPatternFinder("\xE8????\x48\x89\xDE\x5B\x48\x8B\x10").add(1).relativeToAbsolute().get();
    insertIntoTree = ReturnAddress{ clientPatternFinder("\x74\x24\x4C\x8B\x10").add(31).get() };
    uiComponentInventory = clientPatternFinder("\xE8????\x4C\x89\x3D????\x4C\x89\xFF\xEB\x9E").add(8).relativeToAbsolute().as<decltype(uiComponentInventory)>();
    setItemSessionPropertyValue = clientPatternFinder("\xE8????\x48\x8B\x85????\x41\x83\xC4\x01").add(1).relativeToAbsolute().get();

    localPlayer.init(clientPatternFinder("\x83\xFF\xFF\x48\x8B\x05").add(6).relativeToAbsolute().as<csgo::pod::Entity**>());
#endif
}
