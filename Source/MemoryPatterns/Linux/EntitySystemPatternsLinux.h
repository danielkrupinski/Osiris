#pragma once

#include <MemoryPatterns/PatternTypes/EntitySystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntitySystemPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<EntitySystemPointer, CodePattern{"48 81 EC ? ? ? ? 4C 8D 2D ? ? ? ? EB"}.add(10).abs()>()
            .template addPattern<HighestEntityIndexOffset, CodePattern{"39 97 ? ? ? ? 7D 06"}.add(2).read()>()
            .template addPattern<EntityListOffset, CodePattern{"4C 8D 6F ? 41 54 53 48 89 FB 48 83 EC ? 48 89 07 48"}.add(3).read()>()
            .template addPattern<OffsetToEntityClasses, CodePattern{"86 ? ? ? ? 48 8D 14 D0"}.add(1).read()>();
    }
};
