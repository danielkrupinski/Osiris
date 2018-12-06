#include <stdexcept>
#include <Windows.h>
#include <Psapi.h>

#include "Interfaces.h"
#include "Memory.h"

Memory::Memory() noexcept
{
    /*
    present = findPattern("gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F8 85 DB") + 2;
    reset = findPattern("gameoverlayrenderer.dll", "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B F8") + 9;
    localPlayer = *reinterpret_cast<BaseEntity***>(findPattern("client_panorama.dll", "8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<std::uintptr_t**>(interfaces.client))[10] + 5);
    input = *reinterpret_cast<Input**>(findPattern("client_panorama.dll", "B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85") + 1);
    glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern("client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3);
    disablePostProcessing = *reinterpret_cast<bool**>(findPattern("client_panorama.dll", "80 3D ? ? ? ? ? 53 56 57 0F 85") + 2);
    loadSky = findPattern("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45");
    setClanTag = findPattern("engine.dll", "53 56 57 8B DA 8B F9 FF 15");
    smokeCount = *reinterpret_cast<int**>(findPattern("client_panorama.dll", "8B 1D ? ? ? ? 56 33 F6 57 85 DB") + 2);
    */

    present = findPattern_2("gameoverlayrenderer.dll", std::regex{ "\xFF\x15.{4}\x8B\xF8\x85\xDB", std::regex::optimize }) + 2;
    reset = findPattern_2("gameoverlayrenderer.dll", std::regex{ "\xC7\x45.{5}\xFF\x15.{4}\x8B\xF8", std::regex::optimize }) + 9;
    localPlayer = *reinterpret_cast<BaseEntity***>(findPattern_2("client_panorama.dll", std::regex{ "\x8B\x0D.{4}\x83\xFF\xFF\x74\x07", std::regex::optimize }) + 2);
    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<std::uintptr_t**>(interfaces.client))[10] + 5);
    input = *reinterpret_cast<Input**>(findPattern_2("client_panorama.dll", std::regex{ "\xB9.{4}\x8B\x40\x38\xFF\xD0\x84\xC0\x0F\x85", std::regex::optimize }) + 1);
    glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern_2("client_panorama.dll", std::regex{ "\x0F\x11\x05.{4}\x83\xC8\x01", std::regex::optimize }) + 3);
    disablePostProcessing = *reinterpret_cast<bool**>(findPattern_2("client_panorama.dll", std::regex{ "\x80\x3D.{5}\x53\x56\x57\x0F\x85", std::regex::optimize }) + 2);
    loadSky = findPattern_2("engine.dll", std::regex{ "\x55\x8B\xEC\x81\xEC.{4}\x56\x57\x8B\xF9\xC7\x45", std::regex::optimize });
    setClanTag = findPattern_2("engine.dll", std::regex{ "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", std::regex::optimize });
    smokeCount = *reinterpret_cast<int**>(findPattern_2("client_panorama.dll", std::regex{ "\x8B\x1D.{4}\x56\x33\xF6\x57\x85\xDB", std::regex::optimize }) + 2);

}

std::uintptr_t Memory::findPattern(const std::string_view module, const std::string_view pattern) const
{
    MODULEINFO moduleInfo{ };

    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module.data()), &moduleInfo, sizeof(moduleInfo)))
        return 0;

    const std::uintptr_t start_address = reinterpret_cast<std::uintptr_t>(moduleInfo.lpBaseOfDll);
    const std::uintptr_t end_address = start_address + moduleInfo.SizeOfImage;
    const char* scanPattern = pattern.data();

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
            scanPattern = pattern.data();
            first_match = 0;
        }
    }
    return 0;
}

std::uintptr_t Memory::findPattern_2(const std::string_view module, const std::regex pattern) const
{
    MODULEINFO moduleInfo;

    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module.data()), &moduleInfo, sizeof(moduleInfo)))
        throw std::runtime_error("Could not get module information about " + std::string{ module } + "!");
       // return 0; // TODO: Error / exception throw

    const char* begin = reinterpret_cast<const char*>(moduleInfo.lpBaseOfDll);
    const char* end = begin + moduleInfo.SizeOfImage;

    std::cmatch match;
    if (std::regex_search(begin, end, match, pattern))
        return reinterpret_cast<ptrdiff_t>(moduleInfo.lpBaseOfDll) + match.position();
    else
        throw std::runtime_error("Memory pattern search failed!");
}
