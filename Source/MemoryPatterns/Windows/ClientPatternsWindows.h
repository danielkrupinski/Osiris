#pragma once

#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ClientPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<MainMenuPanelPointer, CodePattern{"48 83 EC ? 48 8B 05 ? ? ? ? 48 8D 15"}.add(7).abs()>()
            .template addPattern<HudPanelPointer, CodePattern{"48 89 35 ? ? ? ? E8 ? ? ? ? 48 85"}.add(3).abs()>()
            .template addPattern<GlobalVarsPointer, CodePattern{"48 8B 05 ? ? ? ? 0F 57 C0 8B 48"}.add(3).abs()>()
            .template addPattern<TransformTranslate3dVMT, CodePattern{"00 00 80 00 48 8D 05 ? ? ? ? 48 C7 42 ? 00"}.add(7).abs()>()
            .template addPattern<TransformScale3dVMT, CodePattern{"48 8D 0D ? ? ? ? F3 0F 10 4B ? F3 0F 10 43"}.add(3).abs()>()
            .template addPattern<WorldToProjectionMatrixPointer, CodePattern{"48 8D 0D ? ? ? ? 48 C1 E0 06"}.add(3).abs()>()
            .template addPattern<ViewToProjectionMatrixPointer, CodePattern{"48 89 4C 24 ? 4C 8D 0D ? ? ? ? 48 8B 0D"}.add(8).abs()>()
            .template addPattern<ViewRenderPointer, CodePattern{"48 89 05 ? ? ? ? 48 8B C8 48 85 C0"}.add(3).abs()>()
            .template addPattern<LocalPlayerControllerPointer, CodePattern{"48 83 3D ? ? ? ? 00 0F 95"}.add(3).abs(5)>()
            .template addPattern<ManageGlowSceneObjectPointer, CodePattern{"E8 ? ? ? ? 48 8B 4F ? 0F 28 7C"}.add(1).abs()>()
            .template addPattern<SetSceneObjectAttributeFloat4, CodePattern{"E8 ? ? ? ? FF C6 48 83 C3 ? 49 3B"}.add(1).abs()>()
            .template addPattern<PointerToClientMode, CodePattern{"57 48 83 EC ? 33 DB 48 8D 3D ? ? ? ? 48 8D"}.add(10).abs()>();
    }
};
