#pragma once

#include <MemoryPatterns/PatternTypes/MapPlayerPreviewPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MapPlayerPreviewPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPreviewPanelEntities, CodePattern{"43 ? 4A 8D 04 B8"}.add(1).read()>();
    }
};
