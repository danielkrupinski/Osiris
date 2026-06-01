#pragma once

#include <CS2/Constants/DllNames.h>
#include <Hooks/PeepEventsHook.h>
#include <MemoryPatterns/MemoryPatterns.h>
#include <MemoryPatterns/PatternFinders.h>
#include <MemorySearch/PatternFinder.h>
#include <MemorySearch/PatternNotFoundLogger.h>
#include <Platform/DynamicLibrary.h>
#include <SDL/SdlDll.h>

struct PartialGlobalContext {
    explicit PartialGlobalContext(DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, SdlDll sdlDLL) noexcept
        : patternFinders{
            PatternFinder<PatternNotFoundLogger>{clientDLL.getCodeSection().raw(), "client.dll"},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::TIER0_DLL}.getCodeSection().raw(), "tier0.dll"},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::SOUNDSYSTEM_DLL}.getCodeSection().raw(), "soundsystem.dll"},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::FILESYSTEM_DLL}.getCodeSection().raw(), "filesystem.dll"},
            PatternFinder<PatternNotFoundLogger>{panoramaDLL.getCodeSection().raw(), "panorama.dll"},
            PatternFinder<PatternNotFoundLogger>{sdlDLL.getCodeSection().raw(), "sdl"},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::SCENESYSTEM_DLL}.getCodeSection().raw(), "scenesystem.dll"}}
        , clientDLL{clientDLL}
        , panoramaDLL{panoramaDLL}
        , peepEventsHook{MemoryPatterns{patternFinders}.sdlPatterns().peepEventsPointer(sdlDLL.peepEvents())}
    {
    }

    PatternFinders patternFinders;
    DynamicLibrary clientDLL;
    DynamicLibrary panoramaDLL;
    PeepEventsHook peepEventsHook;

    void enableIfValid() noexcept
    {
        if (peepEventsHook.isValid())
            peepEventsHook.enable();
    }
};
