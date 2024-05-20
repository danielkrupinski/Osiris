#pragma once

#include <MemorySearch/BytePatternLiteral.h>
#include <SDL/SdlFunctions.h>

template <typename PatternFinders>
struct SdlPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] sdl3::SDL_PeepEvents** peepEventsPointer(sdl3::SDL_PeepEvents* peepEvents) const noexcept
    {
        return patternFinders.sdlPatternFinder.matchPatternAtAddress((void*)peepEvents, "48 FF 25 ? ? ? ?"_pat).add(3).abs().template as<sdl3::SDL_PeepEvents**>();
    }
};
