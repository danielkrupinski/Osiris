#pragma once

#include <MemoryPatterns/PatternTypes/EntitySystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntitySystemPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<EntitySystemPointer, CodePattern{"48 8B 0D ? ? ? ? 48 8D 94 24 ? ? ? ? 33 DB"}.add(3).abs()>()
            .template addPattern<HighestEntityIndexOffset, CodePattern{"3B 8F ? ? ? ? 7E 06"}.add(2).read()>()
            .template addPattern<EntityListOffset, CodePattern{"48 8D 4B ? E8 ? ? ? ? 8D 85"}.add(3).read()>()
            .template addPattern<OffsetToEntityClasses, CodePattern{"49 8B 85 ? ? ? ? 44 0F"}.add(3).read()>();
    }
};
