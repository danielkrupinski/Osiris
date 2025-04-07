#pragma once

#include <MemoryPatterns/PatternTypes/MapPreviewPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MapPreviewPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToMapPreviewPanelEntities, CodePattern{"43 ? 4A 8D 04 B8"}.add(1).read()>();
    }
};
