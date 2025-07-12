#pragma once

#include <MemoryPatterns/PatternTypes/TextEntryPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct TextEntryPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<TextEntrySetTextFunction, CodePattern{"8B 89 ? ? ? ? E8 ? ? ? ? B0"}.add(7).abs()>();
    }
};
