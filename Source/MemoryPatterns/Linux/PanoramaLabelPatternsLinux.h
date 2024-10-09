#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaLabelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaLabelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<LabelPanelConstructorPointer, CodePattern{"49 89 C7 E8 ? ? ? ? 49 8B 7C 24 ? 48 8B 07"}.add(4).abs()>()
            .template addPattern<SetLabelTextFunctionPointer, CodePattern{"85 ? ? ? ? 31 C9 E8 ? ? ? ?"}.add(8).abs()>()
            .template addPattern<LabelPanelObjectSize, CodePattern{"BF ? ? ? ? E8 ? ? ? ? 31 D2 4C 89 E6 48 89 C7 49 89 C7 E8 ? ? ? ? 49 8B 7C"}.add(1).read()>();
    }
};
