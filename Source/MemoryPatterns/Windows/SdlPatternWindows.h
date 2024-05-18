#pragma once

#include <MemorySearch/BytePatternLiteral.h>
#include <SDL/SdlFunctions.h>

template <typename SdlPatternFinder>
struct SdlPatterns {
    // FIXME: use PatternFinders, requires change to GlobalContext initialization
    const SdlPatternFinder& sdlPatternFinder;

    [[nodiscard]] sdl3::SDL_PeepEvents** peepEventsPointer(sdl3::SDL_PeepEvents* peepEvents) const noexcept
    {
        return sdlPatternFinder.matchPatternAtAddress((void*)peepEvents, "48 FF 25 ? ? ? ?"_pat).add(3).abs().template as<sdl3::SDL_PeepEvents**>();
    }
};
