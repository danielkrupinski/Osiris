#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetImageFunctionPointer, CodePattern{"C7 45 A8 ? ? ? ? E8 ? ? ? ? 48 83 7D ? 00 74 ? 48 8D"}.add(8).abs()>()
            .template addPattern<ImagePanelConstructorPointer, CodePattern{"E8 ? ? ? ? EB 03 48 8B ? BA"}.add(1).abs()>()
            .template addPattern<ImagePanelClassSize, CodePattern{"01 FF 90 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 48"}.add(8).read()>()
            .template addPattern<ImagePropertiesOffset, CodePattern{"EB 08 41 C7 47 ? 00 00 80 BF ? 8D ? ?"}.add(13).read()>()
            .template addPattern<OffsetToImagePath, CodePattern{"B6 DA 48 81 C1 ? ? ? ?"}.add(5).read()>();
    }
};
