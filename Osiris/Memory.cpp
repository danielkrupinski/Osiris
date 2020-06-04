#include "Interfaces.h"
#include "Memory.h"
#include "SDK/LocalPlayer.h"

template <typename T>
static constexpr auto relativeToAbsolute(int* address) noexcept
{
    return reinterpret_cast<T>(reinterpret_cast<char*>(address + 1) + *address);
}

#define FIND_PATTERN(type, ...) \
reinterpret_cast<type>(findPattern(__VA_ARGS__))

Memory::Memory() noexcept
{
    present = findPattern(L"gameoverlayrenderer", "\xFF\x15????\x8B\xF8\x85\xDB", 2);
    reset = findPattern(L"gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xF8", 9);
    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[10] + 5);
    input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(interfaces->client))[16] + 1);
    globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[11] + 10);
    glowObjectManager = *FIND_PATTERN(GlowObjectManager**, L"client", "\x0F\x11\x05????\x83\xC8\x01", 3);
    disablePostProcessing = *FIND_PATTERN(bool**, L"client", "\x83\xEC\x4C\x80\x3D", 5);
    loadSky = relativeToAbsolute<decltype(loadSky)>(FIND_PATTERN(int*, L"engine", "\xE8????\x84\xC0\x74\x2D\xA1", 1));
    setClanTag = FIND_PATTERN(decltype(setClanTag), L"engine", "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15");
    lineGoesThroughSmoke = relativeToAbsolute<decltype(lineGoesThroughSmoke)>(FIND_PATTERN(int*, L"client", "\xE8????\x8B\x4C\x24\x30\x33\xD2", 1));
    smokeCount = *(reinterpret_cast<int**>(lineGoesThroughSmoke) + 2);
    cameraThink = findPattern(L"client", "\x85\xC0\x75\x30\x38\x86");
    acceptMatch = FIND_PATTERN(decltype(acceptMatch), L"client", "\x55\x8B\xEC\x83\xE4\xF8\x8B\x4D\x08\xBA????\xE8????\x85\xC0\x75\x12");
    getSequenceActivity = FIND_PATTERN(decltype(getSequenceActivity), L"client", "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83");
    scopeArc = findPattern(L"client", "\x6A?\xFF\x50\x3C\x8B\x0D????\xFF\xB7", 5);
    scopeLens = findPattern(L"client", "\xFF\x50\x3C\x8B\x4C\x24\x20", 3);
    isOtherEnemy = relativeToAbsolute<decltype(isOtherEnemy)>(FIND_PATTERN(int*, L"client", "\xE8????\x02\xC0", 1));
    auto temp = FIND_PATTERN(std::uintptr_t*, L"client", "\xB9????\xE8????\x8B\x5D\x08", 1);
    hud = *temp;
    findHudElement = relativeToAbsolute<decltype(findHudElement)>(reinterpret_cast<int*>(reinterpret_cast<char*>(temp) + 5));
    clearHudWeapon = FIND_PATTERN(decltype(clearHudWeapon), L"client", "\x55\x8B\xEC\x51\x53\x56\x8B\x75\x08\x8B\xD9\x57\x6B\xFE\x2C");
    itemSystem = relativeToAbsolute<decltype(itemSystem)>(FIND_PATTERN(int*, L"client", "\xE8????\x0F\xB7\x0F", 1));
    setAbsOrigin = relativeToAbsolute<decltype(setAbsOrigin)>(FIND_PATTERN(int*, L"client", "\xE8????\xEB\x19\x8B\x07", 1));
    listLeaves = findPattern(L"client", "\x56\x52\xFF\x50\x18", 5);
    dispatchSound = FIND_PATTERN(int*, L"engine", "\x74\x0B\xE8????\x8B\x3D", 3);
    traceToExit = FIND_PATTERN(decltype(traceToExit), L"client", "\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75");
    viewRender = **FIND_PATTERN(ViewRender***, L"client", "\x8B\x0D????\xFF\x75\x0C\x8B\x45\x08", 2);
    drawScreenEffectMaterial = relativeToAbsolute<uintptr_t>(FIND_PATTERN(int*, L"client", "\xE8????\x83\xC4\x0C\x8D\x4D\xF8", 1));
    submitReport = FIND_PATTERN(decltype(submitReport), L"client", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x28\x8B\x4D\x08");
    test = relativeToAbsolute<uintptr_t>(FIND_PATTERN(int*, L"client", "\xE8????\x3B\x44\x24\x0C", 1)) + 0x71;
    test2 = findPattern(L"client", "\x85\xC0\x0F\x84????\x80\x78\x10\x00\x0F\x84");
    fakePrime = FIND_PATTERN(std::uint8_t*, L"client", "\x17\xF6\x40\x14\x10") - 1;
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(GetModuleHandleW(L"tier0"), "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(GetModuleHandleW(L"tier0"), "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    vignette = *FIND_PATTERN(float**, L"client", "\x0F\x11\x05????\xF3\x0F\x7E\x87", 3) + 1;
    equipWearable = FIND_PATTERN(decltype(equipWearable), L"client", "\x55\x8B\xEC\x83\xEC\x10\x53\x8B\x5D\x08\x57\x8B\xF9");
    predictionRandomSeed = *FIND_PATTERN(int**, L"client", "\x8B\x0D????\xBA????\xE8????\x83\xC4\x04", 2);
    moveData = **FIND_PATTERN(MoveData***, L"client", "\xA1????\xF3\x0F\x59\xCD", 1);
    moveHelper = **FIND_PATTERN(MoveHelper***, L"client", "\x8B\x0D????\x8B\x45?\x51\x8B\xD4\x89\x02\x8B\x01", 2);
    keyValuesFromString = relativeToAbsolute<decltype(keyValuesFromString)>(FIND_PATTERN(int*, L"client", "\xE8????\x83\xC4\x04\x89\x45\xD8", 1));
    keyValuesFindKey = relativeToAbsolute<decltype(keyValuesFindKey)>(FIND_PATTERN(int*, L"client", "\xE8????\xF7\x45", 1));
    keyValuesSetString = relativeToAbsolute<decltype(keyValuesSetString)>(FIND_PATTERN(int*, L"client", "\xE8????\x89\x77\x38", 1));
    weaponSystem = *reinterpret_cast<WeaponSystem**>(findPattern(L"client", "\x8B\x35????\xFF\x10\x0F\xB7\xC0", 2));

    localPlayer.init(*reinterpret_cast<Entity***>(findPattern(L"client", "\xA1????\x89\x45\xBC\x85\xC0", 1)));
}
