#pragma once

#include <cassert>

namespace cs2
{

using SDL_PeepEvents = int(*)(void* events, int numevents, int action, unsigned minType, unsigned maxType);

}

class PeepEventsHook {
public:
    static int SDL_PeepEvents(void* events, int numevents,
        int action,
        unsigned minType, unsigned maxType) noexcept;

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
        *peepEventsPointer = &SDL_PeepEvents;
    }

    void disable() const noexcept
    {
        assert(isValid());
        *peepEventsPointer = original;
    }

private:
    cs2::SDL_PeepEvents* peepEventsPointer{nullptr};
    cs2::SDL_PeepEvents original{nullptr};
};
