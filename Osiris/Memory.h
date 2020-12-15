#pragma once

#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#elif __linux__
#include <link.h>
#endif

#include "SDK/Platform.h"

class ClientMode;
class Entity;
class GameEventDescriptor;
class GameEventManager;
class Input;
class ItemSystem;
class KeyValues;
class MoveHelper;
class MoveData;
class PlayerResource;
class ViewRender;
class WeaponSystem;
template <typename T>
class UtlVector;

struct ActiveChannels;
struct Channel;
struct GlobalVars;
struct GlowObjectManager;
struct Trace;
struct Vector;

class Memory {
public:
    Memory() noexcept;

#ifdef _WIN32
    uintptr_t present;
    uintptr_t reset;
#else
    uintptr_t pollEvent;
    uintptr_t swapWindow;
#endif

    ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;
    UtlVector<Entity*>* plantedC4s;

    bool* disablePostProcessing;

    std::add_pointer_t<void __FASTCALL(const char*)> loadSky;
    std::add_pointer_t<void __FASTCALL(const char*, const char*)> setClanTag;
    uintptr_t cameraThink;
    std::add_pointer_t<bool __STDCALL(const char*)> acceptMatch;
    std::add_pointer_t<bool __CDECL(Vector, Vector, short)> lineGoesThroughSmoke;
    int(__THISCALL* getSequenceActivity)(void*, int);
    bool(__THISCALL* isOtherEnemy)(Entity*, Entity*);
    uintptr_t hud;
    int*(__THISCALL* findHudElement)(uintptr_t, const char*);
    int(__THISCALL* clearHudWeapon)(int*, int);
    std::add_pointer_t<ItemSystem* __CDECL()> itemSystem;
    void(__THISCALL* setAbsOrigin)(Entity*, const Vector&);
    uintptr_t listLeaves;
    int* dispatchSound;
    uintptr_t traceToExit;
    ViewRender* viewRender;
    uintptr_t drawScreenEffectMaterial;
    uint8_t* fakePrime;
    std::add_pointer_t<void __CDECL(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void __CDECL(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    float* vignette;
    int(__THISCALL* equipWearable)(void* wearable, void* player);
    int* predictionRandomSeed;
    MoveData* moveData;
    MoveHelper* moveHelper;
    std::uintptr_t keyValuesFromString;
    KeyValues*(__THISCALL* keyValuesFindKey)(KeyValues* keyValues, const char* keyName, bool create);
    void(__THISCALL* keyValuesSetString)(KeyValues* keyValues, const char* value);
    WeaponSystem* weaponSystem;
    std::add_pointer_t<const char** __FASTCALL(const char* playerModelName)> getPlayerViewmodelArmConfigForPlayerModel;
    GameEventDescriptor* (__THISCALL* getEventDescriptor)(GameEventManager* _this, const char* name, int* cookie);
    ActiveChannels* activeChannels;
    Channel* channels;
    PlayerResource** playerResource;
    const wchar_t*(__THISCALL* getDecoratedPlayerName)(PlayerResource* pr, int index, wchar_t* buffer, int buffsize, int flags);
    uintptr_t scopeDust;
    uintptr_t scopeArc;
    uintptr_t demoOrHLTV;
    uintptr_t money;
    uintptr_t demoFileEndReached;
    Entity** gameRules;

    bool submitReport(const char* xuid, const char* report) const noexcept
    {
#ifdef _WIN32
        return reinterpret_cast<bool(__stdcall*)(const char*, const char*)>(submitReportFunction)(xuid, report);
#else
        return reinterpret_cast<bool(*)(void*, const char*, const char*)>(submitReportFunction)(nullptr, xuid, report);
#endif
    }
private:
    std::uintptr_t submitReportFunction;

    static std::pair<void*, std::size_t> getModuleInformation(const char* name) noexcept
    {
#ifdef _WIN32
        if (HMODULE handle = GetModuleHandleA(name)) {
            if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo)))
                return std::make_pair(moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);
        }
        return {};
#elif __linux__
        struct ModuleInfo {
            const char* name;
            void* base = nullptr;
            std::size_t size = 0;
        } moduleInfo;

        moduleInfo.name = name;

        dl_iterate_phdr([](struct dl_phdr_info* info, std::size_t, void* data) {
            const auto moduleInfo = reinterpret_cast<ModuleInfo*>(data);
       	    if (std::string_view{ info->dlpi_name }.ends_with(moduleInfo->name)) {
                moduleInfo->base = (void*)(info->dlpi_addr + info->dlpi_phdr[0].p_vaddr);
                moduleInfo->size = info->dlpi_phdr[0].p_memsz;
                return 1;
       	    }
            return 0;
        }, &moduleInfo);
            
        return std::make_pair(moduleInfo.base, moduleInfo.size);
#endif
    }

    static std::uintptr_t findPattern(const char* moduleName, const char* pattern) noexcept
    {
        static auto id = 0;
        ++id;

        const auto [moduleBase, moduleSize] = getModuleInformation(moduleName);

        if (moduleBase && moduleSize) {
            auto start = static_cast<const char*>(moduleBase);
            const auto end = start + moduleSize;

            auto first = start;
            auto second = pattern;

            while (first < end && *second) {
                if (*first == *second || *second == '?') {
                    ++first;
                    ++second;
                } else {
                    first = ++start;
                    second = pattern;
                }
            }

            if (!*second)
                return reinterpret_cast<std::uintptr_t>(start);
        }
#ifdef _WIN32
        MessageBoxA(NULL, ("Failed to find pattern #" + std::to_string(id) + '!').c_str(), "Osiris", MB_OK | MB_ICONWARNING);
#endif
        return 0;
    }
};

inline std::unique_ptr<const Memory> memory;
