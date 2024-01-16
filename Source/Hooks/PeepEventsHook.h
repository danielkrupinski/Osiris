#pragma once

#include <cassert>

namespace cs2
{

using SDL_PeepEvents = int(*)(void* events, int numevents, int action, unsigned minType, unsigned maxType);

}

extern "C" int SDLHook_PeepEvents_asm(void* events, int numevents, int action, unsigned minType, unsigned maxType) noexcept;

class PeepEventsHook {
public:
    explicit PeepEventsHook(cs2::SDL_PeepEvents* peepEvents) noexcept
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

    cs2::SDL_PeepEvents* peepEventsPointer{nullptr};
    cs2::SDL_PeepEvents original{nullptr};
};
