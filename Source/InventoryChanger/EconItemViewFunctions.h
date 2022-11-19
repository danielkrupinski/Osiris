#pragma once

#include <cstdint>

#include <Helpers/PatternFinder.h>
#include <Platform/IsPlatform.h>
#include <SDK/Helpers/EconItemViewFunctions.h>

[[nodiscard]] inline EconItemViewFunctions createEconItemViewFunctions(const helpers::PatternFinder& clientPatternFinder)
{
    return EconItemViewFunctions{
#if IS_WIN32()
        .clearInventoryImageRGBA = clientPatternFinder("\x55\x8B\xEC\x81\xEC????\x57\x8B\xF9\xC7\x47").get(),
        .getSOCData = clientPatternFinder("\xE8????\x32\xC9").add(1).relativeToAbsolute().get()
#elif IS_LINUX()
        .clearInventoryImageRGBA = clientPatternFinder("\xE8????\x83\xC3\x01\x49\x83\xC4\x08\x41\x3B\x5D\x50").add(1).relativeToAbsolute().get(),
        .getSOCData = clientPatternFinder("\xE8????\x5B\x44\x89\xEE").add(1).relativeToAbsolute().get()
#endif
    };
}
