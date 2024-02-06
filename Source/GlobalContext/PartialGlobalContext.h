#pragma once

#include <CS2/Constants/DllNames.h>
#include <Hooks/PeepEventsHook.h>
#include <MemoryPatterns/SdlPatterns.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <SDL/SdlDll.h>

struct PartialGlobalContext {
    explicit PartialGlobalContext(SdlDll sdlDll, SdlPatterns sdlPatterns) noexcept
        : peepEventsHook{sdlPatterns.peepEventsPointer(sdlDll.peepEvents())}
    {
    }

    PeepEventsHook peepEventsHook;

    void enableIfValid() noexcept
    {
        if (peepEventsHook.isValid())
            peepEventsHook.enable();
    }
};
