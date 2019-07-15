#include "Interfaces.h"
#include "Memory.h"

template <typename T>
static constexpr auto relativeToAbsolute(int* address) noexcept
{
    return reinterpret_cast<T>(reinterpret_cast<char*>(address + 1) + *address);
}

Memory::Memory() noexcept
{
    present = findPattern<>(L"gameoverlayrenderer", "\xFF\x15????\x8B\xF8\x85\xDB", 2);
    reset = findPattern<>(L"gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xF8", 9);
    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[10] + 5);
    input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(interfaces.client))[16] + 1);
    globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[11] + 10);
    glowObjectManager = *findPattern<GlowObjectManager**>(L"client_panorama", "\x0F\x11\x05????\x83\xC8\x01", 3);
    disablePostProcessing = *findPattern<bool**>(L"client_panorama", "\x83\xEC\x4C\x80\x3D", 5);
    loadSky = relativeToAbsolute<decltype(loadSky)>(findPattern<int*>(L"engine", "\xE8????\x84\xC0\x74\x2D\xA1", 1));
    setClanTag = findPattern<decltype(setClanTag)>(L"engine", "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15");
    lineGoesThroughSmoke = relativeToAbsolute<decltype(lineGoesThroughSmoke)>(findPattern<int*>(L"client_panorama", "\xE8????\x8B\x4C\x24\x30\x33\xD2", 1));
    smokeCount = *(reinterpret_cast<int**>(lineGoesThroughSmoke) + 2);
    cameraThink = findPattern<>(L"client_panorama", "\x85\xC0\x75\x30\x38\x86");
    acceptMatch = findPattern<decltype(acceptMatch)>(L"client_panorama", "\x55\x8B\xEC\x83\xE4\xF8\x8B\x4D\x08\xBA????\xE8????\x85\xC0\x75\x12");
    getSequenceActivity = findPattern<decltype(getSequenceActivity)>(L"client_panorama", "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83");
    scopeArc = findPattern<>(L"client_panorama", "\x6A?\xFF\x50\x3C\x8B\x0D????\xFF\xB7", 5);
    scopeLens = findPattern<>(L"client_panorama", "\xFF\x50\x3C\x8B\x4C\x24\x20", 3);
    isOtherEnemy = relativeToAbsolute<decltype(isOtherEnemy)>(findPattern<int*>(L"client_panorama", "\xE8????\x02\xC0", 1));
    auto temp = findPattern<uintptr_t*>(L"client_panorama", "\xB9????\xE8????\x8B\x5D\x08", 1);
    hud = *temp;
    findHudElement = relativeToAbsolute<decltype(findHudElement)>(reinterpret_cast<int*>(reinterpret_cast<char*>(temp) + 5));
    clearHudWeapon = findPattern<decltype(clearHudWeapon)>(L"client_panorama", "\x55\x8B\xEC\x51\x53\x56\x8B\x75\x08\x8B\xD9\x57\x6B\xFE\x2C");
    itemSchema = relativeToAbsolute<decltype(itemSchema)>(findPattern<int*>(L"client_panorama", "\xE8????\x0F\xB7\x0F", 1));
    setAbsOrigin = relativeToAbsolute<decltype(setAbsOrigin)>(findPattern<int*>(L"client_panorama", "\xE8????\xEB\x19\x8B\x07", 1));
    listLeaves = findPattern<>(L"client_panorama", "\x56\x52\xFF\x50\x18", 5);
    dispatchSound = findPattern<int*>(L"engine", "\x74\x0B\xE8????\x8B\x3D", 3);
    traceToExit = findPattern<decltype(traceToExit)>(L"client_panorama", "\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75");
    viewRender = **findPattern<ViewRender***>(L"client_panorama", "\x8B\x0D????\xFF\x75\x0C\x8B\x45\x08", 2);
    drawScreenEffectMaterial = relativeToAbsolute<decltype(drawScreenEffectMaterial)>(findPattern<int*>(L"client_panorama", "\xE8????\x83\xC4\x0C\x8D\x4D\xF8", 1));
}
