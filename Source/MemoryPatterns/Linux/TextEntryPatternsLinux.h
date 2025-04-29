#pragma once

#include <MemoryPatterns/PatternTypes/TextEntryPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct TextEntryPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<TextEntrySetTextFunction, CodePattern{"8B BF ? ? ? ? E8 ? ? ? ? B8"}.add(7).abs()>();
    }
};
