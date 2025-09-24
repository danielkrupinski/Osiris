#pragma once

#include <MemoryPatterns/PatternTypes/EntitySystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntitySystemPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<EntitySystemPointer, CodePattern{"48 89 ? ? ? ? ? 4C 63 ? ? ? ? ? 44 3B ? ? ? ? ? 0F"}.add(3).abs()>()
            .template addPattern<EntityListOffset, CodePattern{"48 8D ? ? E8 ? ? ? ? 8D 85"}.add(3).read()>()
            .template addPattern<OffsetToEntityClasses, CodePattern{"8B 86 ? ? ? ? ? 8B ? ? ? FF 15"}.add(2).read()>();
    }
};
