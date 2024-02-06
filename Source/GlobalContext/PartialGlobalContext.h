#pragma once

#include <CS2/Constants/DllNames.h>
#include <Hooks/PeepEventsHook.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <MemoryPatterns/SdlPatterns.h>

struct PartialGlobalContext {
    explicit PartialGlobalContext(DynamicLibrary sdlDll, SdlPatterns sdlPatterns) noexcept
        : peepEventsHook{sdlPatterns.peepEventsPointer(sdlDll.getFunctionAddress("SDL_PeepEvents").as<sdl3::SDL_PeepEvents*>())}
    {
    }

    PeepEventsHook peepEventsHook;

    void enableIfValid() noexcept
    {
        if (peepEventsHook.isValid())
            peepEventsHook.enable();
    }
};
