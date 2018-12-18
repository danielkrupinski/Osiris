#include <regex>
#include <stdexcept>
#include <Windows.h>
#include <Psapi.h>

#include "Interfaces.h"
#include "Memory.h"

Memory::Memory() noexcept
{
    try {
        present = findPattern("gameoverlayrenderer.dll", "\xFF\x15.{4}\x8B\xF8\x85\xDB") + 2;
        reset = findPattern("gameoverlayrenderer.dll", "\xC7\x45.{5}\xFF\x15.{4}\x8B\xF8") + 9;
        localPlayer = *reinterpret_cast<Entity***>(findPattern("client_panorama.dll", "\x8B\x0D.{4}\x83\xFF\xFF\x74\x07") + 2);
        clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<std::uintptr_t**>(interfaces.client))[10] + 5);
        input = *reinterpret_cast<Input**>(findPattern("client_panorama.dll", "\xB9.{4}\x8B\x40\x38\xFF\xD0\x84\xC0\x0F\x85") + 1);
        glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern("client_panorama.dll", "\x0F\x11\x05.{4}\x83\xC8\x01") + 3);
        disablePostProcessing = *reinterpret_cast<bool**>(findPattern("client_panorama.dll", "\x80\x3D.{5}\x53\x56\x57\x0F\x85") + 2);
        loadSky = findPattern("engine.dll", "\x55\x8B\xEC\x81\xEC.{4}\x56\x57\x8B\xF9\xC7\x45");
        setClanTag = findPattern("engine.dll", "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15");
        smokeCount = *reinterpret_cast<int**>(findPattern("client_panorama.dll", "\x8B\x1D.{4}\x56\x33\xF6\x57\x85\xDB") + 2);
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}

std::uintptr_t Memory::findPattern(const std::string& module, const std::string& pattern) const
{
    MODULEINFO moduleInfo;

    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module.c_str()), &moduleInfo, sizeof(moduleInfo)))
        throw std::runtime_error{ "Could not get module information about " + module + "!" };

    const char* begin = reinterpret_cast<const char*>(moduleInfo.lpBaseOfDll);
    const char* end = begin + moduleInfo.SizeOfImage;

    std::cmatch match;
    if (std::regex_search(begin, end, match, std::regex{ pattern, std::regex::optimize }))
        return reinterpret_cast<ptrdiff_t>(moduleInfo.lpBaseOfDll) + match.position();
    else
        throw std::runtime_error{ "Memory search for " + pattern + " in " + module + " failed!" };
}
