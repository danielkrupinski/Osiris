#pragma once

#include <cassert>

#include <SDL/SdlFunctions.h>

extern "C" int SDLHook_PeepEvents_asm(void* events, int numevents, int action, unsigned minType, unsigned maxType) noexcept;

class PeepEventsHook {
public:
    explicit PeepEventsHook(sdl3::SDL_PeepEvents** peepEvents) noexcept
        : peepEventsPointer{peepEvents}
    {
    }

    [[nodiscard]] bool isValid() const noexcept
    {
        return peepEventsPointer != nullptr;
    }

    void enable() noexcept
    {
        assert(isValid());
        original = *peepEventsPointer;
        *peepEventsPointer = &SDLHook_PeepEvents_asm;
    }

    void disable() const noexcept
    {
        assert(isValid());
        *peepEventsPointer = original;
    }

    sdl3::SDL_PeepEvents** peepEventsPointer{nullptr};
    sdl3::SDL_PeepEvents* original{nullptr};
};
