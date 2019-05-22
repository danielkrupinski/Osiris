#pragma once

#include <sstream>
#include <string_view>
#include <Windows.h>
#include <Psapi.h>

class ClientMode;
class Input;
struct GlobalVars;
struct GlowObjectManager;
struct Vector;
class Entity;

class Memory final {
public:
    Memory() noexcept;

    uintptr_t present;
    uintptr_t reset;

    ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;

    bool* disablePostProcessing;

    std::add_pointer_t<void __fastcall(const char*) noexcept> loadSky;
    std::add_pointer_t<void __fastcall(const char*, const char*) noexcept> setClanTag;
    int* smokeCount;
    uintptr_t cameraThink;
    std::add_pointer_t<bool __stdcall(const char*) noexcept> acceptMatch;
    std::add_pointer_t<bool __cdecl(Vector, Vector, short) noexcept> lineGoesThroughSmoke;
    int(__thiscall* getSequenceActivity)(void*, int);
    uintptr_t scopeArc;
    uintptr_t scopeLens;
    bool(__thiscall* isOtherEnemy)(Entity*, Entity*);

private:
    template <typename T = uintptr_t>
    static auto findPattern(const char* module, std::string_view pattern, size_t offset = 0) noexcept
    {
        MODULEINFO moduleInfo;

        if (GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module), &moduleInfo, sizeof(moduleInfo))) {
            char* begin = static_cast<char*>(moduleInfo.lpBaseOfDll);
            char* end = begin + moduleInfo.SizeOfImage - pattern.length() + 1;

            for (char* c = begin; c != end; c++) {
                bool matched = true;
                auto it = c;

                if (*(c + pattern.length() - 1) != pattern.back())
                    continue;

                for (auto a : pattern) {
                    if (a != '?' && *it != a) {
                        matched = false;
                        break;
                    }
                    it++;
                }
                if (matched)
                    return reinterpret_cast<T>(c + offset);
            }
        }
        MessageBox(NULL, (std::ostringstream{ } << "Failed to find pattern in " << module << '!').str().c_str(), "Error", MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }
};

extern Memory memory;
