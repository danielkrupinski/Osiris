#pragma once

#include <MemoryPatterns/PatternTypes/EntitySystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntitySystemPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<EntitySystemPointer, CodePattern{"4C 63 ? ? ? ? ? 48 89 1D ? ? ? ?"}.add(10).abs()>()
            .template addPattern<EntityListOffset, CodePattern{"4C 8D 6F ? 41 54 53 48 89 FB 48 83 EC ? 48 89 07 48"}.add(3).read()>()
            .template addPattern<OffsetToEntityClasses, CodePattern{"49 8B 8F ? ? ? ? 0F B7"}.add(3).read()>();
    }
};
