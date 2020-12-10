#include "Interfaces.h"
#include "Memory.h"
#include "SDK/LocalPlayer.h"

template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

Memory::Memory() noexcept
{
#ifdef _WIN32
    present = findPattern("gameoverlayrenderer", "\xFF\x15????\x8B\xF8\x85\xDB") + 2;
    reset = findPattern("gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xF8") + 9;

    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[10] + 5);
    input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(interfaces->client))[16] + 1);
    globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[11] + 10);
    glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern(CLIENT_DLL, "\x0F\x11\x05????\x83\xC8\x01") + 3);
    disablePostProcessing = *reinterpret_cast<bool**>(findPattern(CLIENT_DLL, "\x83\xEC\x4C\x80\x3D") + 5);
    loadSky = relativeToAbsolute<decltype(loadSky)>(findPattern(ENGINE_DLL, "\xE8????\x84\xC0\x74\x2D\xA1") + 1);
    setClanTag = reinterpret_cast<decltype(setClanTag)>(findPattern(ENGINE_DLL, "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15"));
    lineGoesThroughSmoke = relativeToAbsolute<decltype(lineGoesThroughSmoke)>(findPattern(CLIENT_DLL, "\xE8????\x8B\x4C\x24\x30\x33\xD2") + 1);
    cameraThink = findPattern(CLIENT_DLL, "\x85\xC0\x75\x30\x38\x86");
    acceptMatch = reinterpret_cast<decltype(acceptMatch)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xE4\xF8\x8B\x4D\x08\xBA????\xE8????\x85\xC0\x75\x12"));
    getSequenceActivity = reinterpret_cast<decltype(getSequenceActivity)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83"));
    isOtherEnemy = relativeToAbsolute<decltype(isOtherEnemy)>(findPattern(CLIENT_DLL, "\x8B\xCE\xE8????\x02\xC0") + 3);
    auto temp = reinterpret_cast<std::uintptr_t*>(findPattern(CLIENT_DLL, "\xB9????\xE8????\x8B\x5D\x08") + 1);
    hud = *temp;
    findHudElement = relativeToAbsolute<decltype(findHudElement)>(reinterpret_cast<uintptr_t>(temp) + 5);
    clearHudWeapon = reinterpret_cast<decltype(clearHudWeapon)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x51\x53\x56\x8B\x75\x08\x8B\xD9\x57\x6B\xFE\x2C"));
    itemSystem = relativeToAbsolute<decltype(itemSystem)>(findPattern(CLIENT_DLL, "\xE8????\x0F\xB7\x0F") + 1);
    setAbsOrigin = relativeToAbsolute<decltype(setAbsOrigin)>(findPattern(CLIENT_DLL, "\xE8????\xEB\x19\x8B\x07") + 1);
    listLeaves = findPattern(CLIENT_DLL, "\x56\x52\xFF\x50\x18") + 5;
    dispatchSound = reinterpret_cast<int*>(findPattern(ENGINE_DLL, "\x74\x0B\xE8????\x8B\x3D") + 3);
    traceToExit = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75");
    viewRender = **reinterpret_cast<ViewRender***>(findPattern(CLIENT_DLL, "\x8B\x0D????\xFF\x75\x0C\x8B\x45\x08") + 2);
    drawScreenEffectMaterial = relativeToAbsolute<uintptr_t>(findPattern(CLIENT_DLL, "\xE8????\x83\xC4\x0C\x8D\x4D\xF8") + 1);
    submitReportFunction = findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x28\x8B\x4D\x08");
    fakePrime = reinterpret_cast<std::uint8_t*>(findPattern(CLIENT_DLL, "\x17\xF6\x40\x14\x10") - 1);
    const auto tier0 = GetModuleHandleW(L"tier0");
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(tier0, "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    vignette = *reinterpret_cast<float**>(findPattern(CLIENT_DLL, "\x0F\x11\x05????\xF3\x0F\x7E\x87") + 3) + 1;
    equipWearable = reinterpret_cast<decltype(equipWearable)>(findPattern(CLIENT_DLL, "\x55\x8B\xEC\x83\xEC\x10\x53\x8B\x5D\x08\x57\x8B\xF9"));
    predictionRandomSeed = *reinterpret_cast<int**>(findPattern(CLIENT_DLL, "\x8B\x0D????\xBA????\xE8????\x83\xC4\x04") + 2);
    moveData = **reinterpret_cast<MoveData***>(findPattern(CLIENT_DLL, "\xA1????\xF3\x0F\x59\xCD") + 1);
    moveHelper = **reinterpret_cast<MoveHelper***>(findPattern(CLIENT_DLL, "\x8B\x0D????\x8B\x45?\x51\x8B\xD4\x89\x02\x8B\x01") + 2);
    keyValuesFromString = relativeToAbsolute<decltype(keyValuesFromString)>(findPattern(CLIENT_DLL, "\xE8????\x83\xC4\x04\x89\x45\xD8") + 1);
    keyValuesFindKey = relativeToAbsolute<decltype(keyValuesFindKey)>(findPattern(CLIENT_DLL, "\xE8????\xF7\x45") + 1);
    keyValuesSetString = relativeToAbsolute<decltype(keyValuesSetString)>(findPattern(CLIENT_DLL, "\xE8????\x89\x77\x38") + 1);
    weaponSystem = *reinterpret_cast<WeaponSystem**>(findPattern(CLIENT_DLL, "\x8B\x35????\xFF\x10\x0F\xB7\xC0") + 2);
    getPlayerViewmodelArmConfigForPlayerModel = relativeToAbsolute<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(findPattern(CLIENT_DLL, "\xE8????\x89\x87????\x6A") + 1);
    getEventDescriptor = relativeToAbsolute<decltype(getEventDescriptor)>(findPattern(ENGINE_DLL, "\xE8????\x8B\xD8\x85\xDB\x75\x27") + 1);
    activeChannels = *reinterpret_cast<ActiveChannels**>(findPattern(ENGINE_DLL, "\x8B\x1D????\x89\x5C\x24\x48") + 2);
    channels = *reinterpret_cast<Channel**>(findPattern(ENGINE_DLL, "\x81\xC2????\x8B\x72\x54") + 2);
    playerResource = *reinterpret_cast<PlayerResource***>(findPattern(CLIENT_DLL, "\x74\x30\x8B\x35????\x85\xF6") + 4);
    getDecoratedPlayerName = relativeToAbsolute<decltype(getDecoratedPlayerName)>(findPattern(CLIENT_DLL, "\xE8????\x66\x83\x3E") + 1);
    scopeDust = findPattern(CLIENT_DLL, "\xFF\x50\x3C\x8B\x4C\x24\x20") + 3;
    scopeArc = findPattern(CLIENT_DLL, "\x8B\x0D????\xFF\xB7????\x8B\x01\xFF\x90????\x8B\x7C\x24\x1C");
    demoOrHLTV = findPattern(CLIENT_DLL, "\x84\xC0\x75\x09\x38\x05");
    money = findPattern(CLIENT_DLL, "\x84\xC0\x75\x0C\x5B");
    demoFileEndReached = findPattern(CLIENT_DLL, "\x8B\xC8\x85\xC9\x74\x1F\x80\x79\x10");

    localPlayer.init(*reinterpret_cast<Entity***>(findPattern(CLIENT_DLL, "\xA1????\x89\x45\xBC\x85\xC0") + 1));
#else
    const auto tier0 = dlopen(TIER0_DLL, RTLD_NOLOAD | RTLD_NOW);
    debugMsg = decltype(debugMsg)(dlsym(tier0, "Msg"));
    conColorMsg = decltype(conColorMsg)(dlsym(tier0, "_Z11ConColorMsgRK5ColorPKcz"));
    dlclose(tier0);

    const auto libSDL = dlopen("libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);
    pollEvent = relativeToAbsolute<uintptr_t>(uintptr_t(dlsym(libSDL, "SDL_PollEvent")) + 3);
    swapWindow = relativeToAbsolute<uintptr_t>(uintptr_t(dlsym(libSDL, "SDL_GL_SwapWindow")) + 3);
    dlclose(libSDL);

    globalVars = *relativeToAbsolute<GlobalVars**>((*reinterpret_cast<std::uintptr_t**>(interfaces->client))[11] + 16);
    itemSystem = relativeToAbsolute<decltype(itemSystem)>(findPattern(CLIENT_DLL, "\xE8????\x4D\x63\xEC") + 1);
    weaponSystem = *relativeToAbsolute<WeaponSystem**>(findPattern(CLIENT_DLL, "\x48\x8B\x58\x10\x48\x8B\x07\xFF\x10") + 12);

    isOtherEnemy = relativeToAbsolute<decltype(isOtherEnemy)>(findPattern(CLIENT_DLL, "\xE8????\x84\xC0\x44\x89\xE2") + 1);
    lineGoesThroughSmoke = reinterpret_cast<decltype(lineGoesThroughSmoke)>(findPattern(CLIENT_DLL, "\x40\x0F\xB6\xFF\x55"));
    getDecoratedPlayerName = relativeToAbsolute<decltype(getDecoratedPlayerName)>(findPattern(CLIENT_DLL, "\xE8????\x8B\x33\x4C\x89\xF7") + 1);
    
    hud = relativeToAbsolute<decltype(hud)>(findPattern(CLIENT_DLL, "\x53\x48\x8D\x3D????\x48\x83\xEC\x10\xE8") + 4);
    findHudElement = relativeToAbsolute<decltype(findHudElement)>(findPattern(CLIENT_DLL, "\xE8????\x48\x8D\x50\xE0") + 1);
    
    disablePostProcessing = relativeToAbsolute<decltype(disablePostProcessing)>(findPattern(CLIENT_DLL, "\x80\x3D?????\x89\xB5") + 2);
    submitReportFunction = findPattern(CLIENT_DLL, "\x55\x48\x89\xF7\x48\x89\xE5\x41\x57\x41\x56\x41\x55\x41\x54\x53\x48\x89\xD3\x48\x83\xEC\x58");
    loadSky = relativeToAbsolute<decltype(loadSky)>(findPattern(ENGINE_DLL, "\xE8????\x84\xC0\x74\xAB") + 1);
    clientMode = *relativeToAbsolute<decltype(clientMode)*>(relativeToAbsolute<uintptr_t>((*reinterpret_cast<uintptr_t**>(interfaces->client))[10] + 12) + 4);
    input = **relativeToAbsolute<Input***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[16] + 3);
    playerResource = relativeToAbsolute<PlayerResource**>(findPattern(CLIENT_DLL, "\x74\x38\x48\x8B\x3D????\x89\xDE") + 5);

    glowObjectManager = relativeToAbsolute<decltype(glowObjectManager)>(relativeToAbsolute<uintptr_t>(findPattern(CLIENT_DLL, "\xE8????\x48\x89\xC3\x8B\x40\x20") + 1) + 12);
    setClanTag = relativeToAbsolute<decltype(setClanTag)>(findPattern(ENGINE_DLL, "\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0") + 1);
    getEventDescriptor = relativeToAbsolute<decltype(getEventDescriptor)>(findPattern(ENGINE_DLL, "\xE8????\x48\x85\xC0\x74\x62") + 1);
    activeChannels = relativeToAbsolute<ActiveChannels*>(findPattern(ENGINE_DLL, "\x48\x8D\x3D????\x4C\x89\xE6\xE8????\x8B\xBD") + 3);
    channels = relativeToAbsolute<Channel*>(findPattern(ENGINE_DLL, "\x4C\x8D\x35????\x49\x83\xC4\x04") + 3);
    cameraThink = findPattern(CLIENT_DLL, "\x85\xC0\x75\x64\x41\x80\x7D");
    getPlayerViewmodelArmConfigForPlayerModel = relativeToAbsolute<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(findPattern(CLIENT_DLL, "\xE8????\x48\x8B\x78\x08\xBA") + 1);
    keyValuesFromString = relativeToAbsolute<decltype(keyValuesFromString)>(findPattern(CLIENT_DLL, "\xE8????\x48\x89\xDF\x48\x89\x45\xE0") + 1);
    keyValuesFindKey = relativeToAbsolute<decltype(keyValuesFindKey)>(findPattern(CLIENT_DLL, "\xE8????\x48\x85\xC0\x75\x24") + 1);
    keyValuesSetString = relativeToAbsolute<decltype(keyValuesSetString)>(findPattern(CLIENT_DLL, "\xE8????\x4C\x89\xE6\x4C\x89\xFF\xE8????\x48\x8B\x03") + 1);
    drawScreenEffectMaterial = findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x57\x41\x56\x45\x89\xC6\x41\x55\x41\x54\x53");
    vignette = relativeToAbsolute<float*>(findPattern(CLIENT_DLL, "\x48\x8B\x07\x0F\x2F\x05") + 6);
    viewRender = **relativeToAbsolute<ViewRender***>(findPattern(CLIENT_DLL, "\x0F\x85????\x48\x8B\x05????\x45\x89\xF8") + 9);
    scopeDust = findPattern(CLIENT_DLL, "\x8B\x85????\x43\x8D\x14\x2E");
    scopeArc = findPattern(CLIENT_DLL, "\x49\x8B\x3C\x24\x8B\xB3????\x48\x8B\x07\xFF\x90????\x49\x8B\x3C\x24\x4C\x89\xEA");
    demoOrHLTV = findPattern(CLIENT_DLL, "\x0F\xB6\x10\x89\xD0") - 16;
    money = findPattern(CLIENT_DLL, "\x84\xC0\x75\x9E\xB8????\xEB\xB9");
    demoFileEndReached = findPattern(CLIENT_DLL, "\x48\x85\xC0\x0F\x84????\x80\x78\x10?\x74\x7F");
    clearHudWeapon = relativeToAbsolute<decltype(clearHudWeapon)>(findPattern(CLIENT_DLL, "\xE8????\xC6\x45\xB8\x01") + 1);
    equipWearable = reinterpret_cast<decltype(equipWearable)>(findPattern(CLIENT_DLL, "\x55\x48\x89\xE5\x41\x56\x41\x55\x41\x54\x49\x89\xF4\x53\x48\x89\xFB\x48\x83\xEC\x10\x48\x8B\x07"));

    localPlayer.init(relativeToAbsolute<Entity**>(findPattern(CLIENT_DLL, "\x83\xFF\xFF\x48\x8B\x05") + 6));
#endif
}
