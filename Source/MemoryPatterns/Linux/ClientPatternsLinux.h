#pragma once

#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ClientPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<MainMenuPanelPointer, CodePattern{"83 FB ? 75 ? 48 8D 05 ? ? ? ? 48 8B 00"}.add(8).abs()>()
            .template addPattern<HudPanelPointer, CodePattern{"41 C6 84 24 ? ? ? ? ? 4C 89 25 ? ? ? ? E8"}.add(12).abs()>()
            .template addPattern<GlobalVarsPointer, CodePattern{"8D ? ? ? ? ? 48 89 35 ? ? ? ? 48 89 ? ? C3"}.add(9).abs()>()
            .template addPattern<TransformTranslate3dVMT, CodePattern{"48 8D 0D ? ? ? ? 48 89 08 48 89 50 08 48 8B 53 10"}.add(3).abs()>()
            .template addPattern<TransformScale3dVMT, CodePattern{"48 8B 53 08 48 8D 0D ? ? ? ? F3 0F 10 43"}.add(7).abs()>()
            .template addPattern<WorldToProjectionMatrixPointer, CodePattern{"48 8D 05 ? ? ? ? 4C 8D 05 ? ? ? ? 48 8D 0D"}.add(10).abs()>()
            .template addPattern<ViewToProjectionMatrixPointer, CodePattern{"48 8D 0D ? ? ? ? 48 8B 38 E8"}.add(3).abs()>()
            .template addPattern<ViewRenderPointer, CodePattern{"48 8D 05 ? ? ? ? 48 89 38 48 85"}.add(3).abs()>()
            .template addPattern<LocalPlayerControllerPointer, CodePattern{"48 83 3D ? ? ? ? ? 0F 95 C0 C3"}.add(3).abs(5)>()
            .template addPattern<ManageGlowSceneObjectPointer, CodePattern{"E8 ? ? ? ? 4C 39 F3 48"}.add(1).abs()>()
            .template addPattern<SetSceneObjectAttributeFloat4, CodePattern{"55 66 0F 6E DE 48 89 E5 41 55 49"}>();
    }
};
