#pragma once

#include <MemoryPatterns/SdlPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

sdl3::SDL_PeepEvents** SdlPatterns::peepEventsPointer(sdl3::SDL_PeepEvents* peepEvents) const noexcept
{
    return sdlPatternFinder.matchPatternAtAddress((void*)peepEvents, "48 FF 25 ? ? ? ?"_pat).add(3).abs().as<sdl3::SDL_PeepEvents**>();
}
