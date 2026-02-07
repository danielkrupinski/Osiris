#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaLabelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaLabelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<LabelPanelConstructorPointer, CodePattern{"EC ? E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 89 DE 48 89 C7 49 89 C4 E8 ? ? ? ?"}.add(24).abs()>()
            .template addPattern<SetLabelTextFunctionPointer, CodePattern{"0F 85 ? ? ? ? 31 C9 E8 ? ? ? ?"}.add(9).abs()>()
            .template addPattern<LabelPanelObjectSize, CodePattern{"? ? ? ? 48 83 EC ? E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 89 DE"}.read()>();
    }
};
