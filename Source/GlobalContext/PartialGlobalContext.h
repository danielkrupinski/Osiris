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
            PatternFinder<PatternNotFoundLogger>{clientDLL.getCodeSection().raw()},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::TIER0_DLL}.getCodeSection().raw()},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::SOUNDSYSTEM_DLL}.getCodeSection().raw()},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::FILESYSTEM_DLL}.getCodeSection().raw()},
            PatternFinder<PatternNotFoundLogger>{panoramaDLL.getCodeSection().raw()},
            PatternFinder<PatternNotFoundLogger>{sdlDLL.getCodeSection().raw()},
            PatternFinder<PatternNotFoundLogger>{DynamicLibrary{cs2::SCENESYSTEM_DLL}.getCodeSection().raw()}}
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
