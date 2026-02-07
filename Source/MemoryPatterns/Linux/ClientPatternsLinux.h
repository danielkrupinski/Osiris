#pragma once

#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ClientPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<MainMenuPanelPointer, CodePattern{"E5 53 48 83 EC ? 48 8D 1D ? ? ? ? 48 8B 03 48 8B 78 ? 48 8B"}.add(9).abs()>()
            .template addPattern<HudPanelPointer, CodePattern{"05 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 40"}.add(1).abs()>()
            .template addPattern<GlobalVarsPointer, CodePattern{"8D ? ? ? ? ? 48 89 35 ? ? ? ? 48 89 ? ? C3"}.add(9).abs()>()
            .template addPattern<TransformTranslate3dVMT, CodePattern{"48 8D 0D ? ? ? ? 48 89 08 48 89 50 08 48 8B 53 10"}.add(3).abs()>()
            .template addPattern<TransformScale3dVMT, CodePattern{"48 8B 53 08 48 8D 0D ? ? ? ? F3 0F 10 43"}.add(7).abs()>()
            .template addPattern<WorldToProjectionMatrixPointer, CodePattern{"01 4C 8D 05 ? ? ? ? 4C 89 EE"}.add(4).abs()>()
            .template addPattern<ViewToProjectionMatrixPointer, CodePattern{"EE 48 8D 0D ? ? ? ? 48 8D 15 ? ? ? ? 48"}.add(4).abs()>()
            .template addPattern<ViewRenderPointer, CodePattern{"48 8D 05 ? ? ? ? 48 89 38 48 85"}.add(3).abs()>()
            .template addPattern<LocalPlayerControllerPointer, CodePattern{"48 83 3D ? ? ? ? ? 0F 95 C0 C3"}.add(3).abs(5)>()
            .template addPattern<ManageGlowSceneObjectPointer, CodePattern{"55 66 48 0F 7E C8"}>()
            .template addPattern<SetSceneObjectAttributeFloat4, CodePattern{"55 66 0F 6E D6 48 89 E5 53 48"}>()
            .template addPattern<PointerToClientMode, CodePattern{"05 ? ? ? ? ? 89 ? 48 89 05 ? ? ? ? E8 ? ? ? ? ? 8B ? ? C9"}.add(1).abs()>();
    }
};
