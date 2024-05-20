#pragma once

#include <MemorySearch/BytePatternLiteral.h>
#include <SDL/SdlFunctions.h>

template <typename PatternFinders>
struct SdlPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] sdl3::SDL_PeepEvents** peepEventsPointer(sdl3::SDL_PeepEvents* peepEvents) const noexcept
    {
        return patternFinders.sdlPatternFinder.matchPatternAtAddress((void*)peepEvents, "FF 25 ? ? ? ?"_pat).add(2).abs().template as<sdl3::SDL_PeepEvents**>();
    }
};
