#pragma once

#include <CS2/Constants/DllNames.h>
#include <Hooks/PeepEventsHook.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/Macros/PlatformSpecific.h>

struct PartialGlobalContext {
    PeepEventsHook peepEventsHook{DynamicLibrary{cs2::SDL_DLL}.getFunctionAddress("SDL_PeepEvents").add(WIN32_LINUX(3, 2)).abs().as<cs2::SDL_PeepEvents*>()};

    void enableIfValid() noexcept
    {
        if (peepEventsHook.isValid())
            peepEventsHook.enable();
    }
};
