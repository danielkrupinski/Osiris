#pragma once

#include <MemorySearch/BytePatternLiteral.h>
#include <SDL/SdlFunctions.h>

template <typename SdlPatternFinder>
struct SdlPatterns {
    // FIXME: use PatternFinders, requires change to GlobalContext initialization
    const SdlPatternFinder& sdlPatternFinder;

    [[nodiscard]] sdl3::SDL_PeepEvents** peepEventsPointer(sdl3::SDL_PeepEvents* peepEvents) const noexcept
    {
        return sdlPatternFinder.matchPatternAtAddress((void*)peepEvents, "FF 25 ? ? ? ?"_pat).add(2).abs().template as<sdl3::SDL_PeepEvents**>();
    }
};
