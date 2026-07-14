#pragma once

#include <MemoryPatterns/PatternTypes/EntitySystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntitySystemPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<EntitySystemPointer, CodePattern{"48 89 ? ? ? ? ? 4C 63 ? ? ? ? ? 44 3B ? ? ? ? ? 0F"}.add(3).abs()>()
            .template addPattern<EntityListOffset, CodePattern{"48 8D ? ? E8 ? ? ? ? 8D 85"}.add(3).read()>()
            .template addPattern<FindEntityClassFunctionPointer, CodePattern{"48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8D 4B 18 4C 8D 4D 48 48 89"}.add(8).abs()>()
            .template addPattern<EntityClassParentNodeOffset, CodePattern{"48 8B 49 ? 48 8B 49 ? 48 3B C8 74 ? 48 83 79 ? ? 75 ? 33 DB"}.add(3).read()>()
            .template addPattern<EntityClassParentOuterOffset, CodePattern{"48 8B 49 ? 48 8B 49 ? 48 3B C8 74 ? 48 83 79 ? ? 75 ? 33 DB"}.add(7).read()>()
            .template addPattern<EntityClassParentNodeOffsetConsistency, CodePattern{"48 3B C8 74 ? 48 83 79 ? 00 75 ? 33 DB"}.add(8).read()>();
    }
};
