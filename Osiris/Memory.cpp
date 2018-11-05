#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Psapi.h>

#include "xorstr.hpp"

#include "Interfaces.h"
#include "Memory.h"

Memory::Memory() noexcept
{
    present = findPattern(xorstr_("gameoverlayrenderer.dll"), xorstr_("FF 15 ? ? ? ? 8B F8 85 DB")) + 2;
    reset = findPattern(xorstr_("gameoverlayrenderer.dll"), xorstr_("C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B F8")) + 9;
    globalVars = **reinterpret_cast<GlobalVars***>(findPattern(xorstr_("client_panorama.dll"), xorstr_("A1 ? ? ? ? 5E 8B 40 10")) + 1);
    localPlayer = *reinterpret_cast<BaseEntity***>(findPattern(xorstr_("client_panorama.dll"), xorstr_("8B 0D ? ? ? ? 83 FF FF 74 07")) + 2);
    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<std::uintptr_t**>(interfaces.client))[10] + 5);
    input = *reinterpret_cast<Input**>(findPattern(xorstr_("client_panorama.dll"), xorstr_("B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85")) + 1);
    glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern(xorstr_("client_panorama.dll"), xorstr_("0F 11 05 ? ? ? ? 83 C8 01")) + 3);
    disablePostProcessing = *reinterpret_cast<bool**>(findPattern(xorstr_("client_panorama.dll"), xorstr_("80 3D ? ? ? ? ? 53 56 57 0F 85")) + 2);
    loadSky = findPattern(xorstr_("engine.dll"), xorstr_("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
    setClanTag = findPattern(xorstr_("engine.dll"), xorstr_("53 56 57 8B DA 8B F9 FF 15"));
    smokeCount = *reinterpret_cast<int**>(findPattern(xorstr_("client_panorama.dll"), xorstr_("8B 1D ? ? ? ? 56 33 F6 57 85 DB")) + 2);
}

std::uintptr_t Memory::findPattern(std::string module, std::string pattern) const
{
    MODULEINFO moduleInfo{ };

    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module.c_str()), &moduleInfo, sizeof(moduleInfo)))
        return 0;

    const std::uintptr_t start_address = reinterpret_cast<std::uintptr_t>(moduleInfo.lpBaseOfDll);
    const std::uintptr_t end_address = start_address + moduleInfo.SizeOfImage;
    const char* scanPattern = pattern.c_str();

    std::uintptr_t first_match = 0;

    for (std::uintptr_t position = start_address; position < end_address; position++) {
        if (!*scanPattern)
            return first_match;

        const std::uint8_t pattern_current = *reinterpret_cast<const std::uint8_t*>(scanPattern);
        const std::uint8_t memory_current = *reinterpret_cast<const std::uint8_t*>(position);

        if (pattern_current == '\?' || memory_current == ((((scanPattern[0] & (~0x20)) >= 'A' && (scanPattern[0] & (~0x20)) <= 'F') ? ((scanPattern[0] & (~0x20)) - 'A' + 0xA) : ((scanPattern[0] >= '0' && scanPattern[0] <= '9') ? scanPattern[0] - '0' : 0)) << 4 | (((scanPattern[1] & (~0x20)) >= 'A' && (scanPattern[1] & (~0x20)) <= 'F') ? ((scanPattern[1] & (~0x20)) - 'A' + 0xA) : ((scanPattern[1] >= '0' && scanPattern[1] <= '9') ? scanPattern[1] - '0' : 0)))) {
            if (!first_match)
                first_match = position;

            if (!scanPattern[2])
                return first_match;

            scanPattern += pattern_current != '\?' ? 3 : 2;
        }
        else {
            scanPattern = pattern.c_str();
            first_match = 0;
        }
    }
    return 0;
}
