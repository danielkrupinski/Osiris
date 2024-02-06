#pragma once

#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>
#include <SDL/SdlFunctions.h>

struct SdlPatterns {
    [[nodiscard]] sdl3::SDL_PeepEvents** peepEventsPointer(sdl3::SDL_PeepEvents* peepEvents) const noexcept;

    const PatternFinder<PatternNotFoundLogger>& sdlPatternFinder;
};
