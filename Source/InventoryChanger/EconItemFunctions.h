#pragma once

#include <cstdint>

#include <Helpers/PatternFinder.h>
#include <Platform/IsPlatform.h>

struct EconItemFunctions {
    std::uintptr_t setDynamicAttributeValue;
    std::uintptr_t removeDynamicAttribute;
};

[[nodiscard]] inline EconItemFunctions createEconItemFunctions(const helpers::PatternFinder& clientPatternFinder)
{
    return EconItemFunctions{
#if IS_WIN32()
        .setDynamicAttributeValue = clientPatternFinder("\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x3C\x53\x8B\x5D\x08\x56\x57\x6A").get(),
        .removeDynamicAttribute = clientPatternFinder("\x55\x8B\xEC\x83\xEC\x08\x8B\xC1\x89\x45\xF8").get()
#elif IS_LINUX()
        .setDynamicAttributeValueFn = clientPatternFinder("\x41\x8B\x06\x49\x8D\x7D\x08").add(-95).get(),
        .removeDynamicAttribute = clientPatternFinder("\xE8????\x80\x3D?????\x75\x14\x48\x8D\x3D????\xE8????\x85\xC0\x0F\x85????\xC7\x45").add(1).relativeToAbsolute().get()
#endif()
    };
}
