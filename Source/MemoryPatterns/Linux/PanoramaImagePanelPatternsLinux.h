#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetImageFunctionPointer, CodePattern{"55 48 89 E5 41 57 49 89 F7 41 56 49 89 D6 41 55 41 54 49 89 CC 53 48 89 FB 48 83"}>()
            .template addPattern<ImagePanelConstructorPointer, CodePattern{"D2 48 89 C7 48 89 85 ? ? ? ? E8 ? ? ? ? ? 8B ? ? ? ? ? ? 8D BD"}.add(12).abs()>()
            .template addPattern<ImagePanelClassSize, CodePattern{"E9 ? ? ? ? BF ? ? ? ? ? 89 ? ? ? ? ? E8 ? ? ? ? 48 8B B5 ? ? ? ? 31"}.add(6).read()>()
            .template addPattern<ImagePropertiesOffset, CodePattern{"? 48 85 D2 48 0F 44 D0 48 85 F6 48 0F 44 F0 E8 ? ? ? ? E9"}.read()>()
            .template addPattern<OffsetToImagePath, CodePattern{"48 81 C7 ? ? ? ? 53 89 F3 48 C7"}.add(3).read()>();
    }
};
