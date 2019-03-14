#include <regex>
#include <stdexcept>
#include <Windows.h>
#include <Psapi.h>

#include "Interfaces.h"
#include "Memory.h"

Memory::Memory() noexcept
{
    try {
        present = findPattern_2("gameoverlayrenderer", "\xFF\x15????\x8B\xF8\x85\xDB") + 2;
        reset = findPattern_2("gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xF8") + 9;
        clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[10] + 5);
        input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(interfaces.client))[16] + 1);
        globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[11] + 10);
        glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern_2("client_panorama", "\x0F\x11\x05????\x83\xC8\x01") + 3);
        disablePostProcessing = *reinterpret_cast<bool**>(findPattern_2("client_panorama", "\x80\x3D?????\x53\x56\x57\x0F\x85") + 2);
        loadSky = findPattern_2("engine", "\x55\x8B\xEC\x81\xEC????\x56\x57\x8B\xF9\xC7\x45");
        setClanTag = reinterpret_cast<decltype(setClanTag)>(findPattern_2("engine", "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15"));
        smokeCount = *reinterpret_cast<int**>(findPattern_2("client_panorama", "\x8B\x1D????\x56\x33\xF6\x57\x85\xDB") + 2);
        cameraThink = findPattern_2("client_panorama", "\x85\xC0\x75\x30\x38\x86");
        revealRanks = reinterpret_cast<decltype(revealRanks)>(findPattern_2("client_panorama", "\x55\x8B\xEC\x8B\x0D????\x85\xC9\x75\x28\xA1????\x68????\x8B\x08\x8B\x01\xFF\x50\x04\x85\xC0\x74\x0B\x8B\xC8\xE8????\x8B\xC8\xEB\x02\x33\xC9\x89\x0D????\x8B\x45\x08"));
        acceptMatch = reinterpret_cast<decltype(acceptMatch)>(findPattern_2("client_panorama", "\x55\x8B\xEC\x83\xE4\xF8\x8B\x4D\x08\xBA????\xE8????\x85\xC0\x75\x12"));
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}

uintptr_t Memory::findPattern(const std::string& module, const std::string& pattern) const
{
    MODULEINFO moduleInfo;

    GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module.c_str()), &moduleInfo, sizeof(moduleInfo));

    const char* begin = reinterpret_cast<const char*>(moduleInfo.lpBaseOfDll);
    const char* end = begin + moduleInfo.SizeOfImage;

    std::cmatch match;

    int searchCount = 0;
    while (!std::regex_search(begin, end, match, std::regex{ pattern, std::regex::optimize }) && searchCount < 5)
        searchCount++;

    if (searchCount == 5)
        throw std::runtime_error{ "Memory pattern scan in " + module + " failed after 5 tries!" };
    else
        return reinterpret_cast<ptrdiff_t>(moduleInfo.lpBaseOfDll) + match.position();
}

uintptr_t Memory::findPattern_2(const char* module, std::string_view pattern) const
{
    MODULEINFO moduleInfo;

    GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module), &moduleInfo, sizeof(moduleInfo));

    const char* begin = static_cast<const char*>(moduleInfo.lpBaseOfDll);
    const char* end = begin + moduleInfo.SizeOfImage;

    for (const char* c = begin; c != end; c++) {
        bool matched = true;
        auto it = c;

        for (auto a : pattern) {
            if (a != '?' && *it != a) {
                matched = false;
                break;
            }
            it++;
        }
        if (matched)
            return reinterpret_cast<uintptr_t>(c);
    }
    throw std::runtime_error{ "Memory scan failed!" };
}