#pragma once

#include <MemoryPatterns/PatternTypes/MapPreviewPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MapPreviewPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToMapPreviewPanelEntities, CodePattern{"F3 0F 11 ? ? ? ? ? 44 39 ? ? 0F"}.add(11).read()>();
    }
};
