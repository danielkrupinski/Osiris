#pragma once

#if IS_WIN64()
#include "Windows/WindowsPatterns.h"
#elif IS_LINUX()
#include "Linux/LinuxPatterns.h"
#endif

#include "PatternFinders.h"

#include <MemorySearch/PatternPool.h>

struct MemoryPatterns {
    const PatternFinders& patternFinders;

#define MEMORY_PATTERNS(type, name) \
[[nodiscard]] auto name() const noexcept \
{ \
    return type<PatternFinders>{patternFinders}; \
}

    MEMORY_PATTERNS(ConVarPatterns, conVarPatterns)
    MEMORY_PATTERNS(CvarPatterns, cvarPatterns)
    MEMORY_PATTERNS(FileSystemPatterns, fileSystemPatterns)
    MEMORY_PATTERNS(PanelStylePatterns, panelStylePatterns)
    MEMORY_PATTERNS(PanoramaUiEnginePatterns, panoramaUiEnginePatterns)
    MEMORY_PATTERNS(PanoramaUiPanelPatterns, panoramaUiPanelPatterns)
    MEMORY_PATTERNS(PlantedC4Patterns, plantedC4Patterns)
    MEMORY_PATTERNS(PlayerControllerPatterns, playerControllerPatterns)
    MEMORY_PATTERNS(PlayerPawnPatterns, playerPawnPatterns)
    MEMORY_PATTERNS(RenderComponentPatterns, renderComponentPatterns)
    MEMORY_PATTERNS(SceneObjectPatterns, sceneObjectPatterns)
    MEMORY_PATTERNS(SceneObjectUpdaterPatterns, sceneObjectUpdaterPatterns)
    MEMORY_PATTERNS(SceneSystemPatterns, sceneSystemPatterns)
    MEMORY_PATTERNS(SdlPatterns, sdlPatterns)
    MEMORY_PATTERNS(SmokeGrenadeProjectilePatterns, smokeGrenadeProjectilePatterns)
    MEMORY_PATTERNS(SoundSystemPatterns, soundSystemPatterns)
    MEMORY_PATTERNS(TopLevelWindowPatterns, topLevelWindowPatterns)
    MEMORY_PATTERNS(WeaponPatterns, weaponPatterns)
    MEMORY_PATTERNS(WeaponServicesPatterns, weaponServicesPatterns)
    MEMORY_PATTERNS(WeaponVDataPatterns, weaponVDataPatterns)

#undef MEMORY_PATTERNS
};

template <typename T>
struct ClientPatternsAdder {
    [[nodiscard]] consteval auto operator()(auto patternPool) const noexcept
    {
        return T::addClientPatterns(patternPool);
    }
};

constexpr auto kClientPatterns = []{
    return PatternPool{}
        .addPatterns(ClientPatternsAdder<ClientPatterns>{})
        .addPatterns(ClientPatternsAdder<EntityPatterns>{})
        .addPatterns(ClientPatternsAdder<EntitySystemPatterns>{})
        .addPatterns(ClientPatternsAdder<GameRulesPatterns>{})
        .addPatterns(ClientPatternsAdder<GameSceneNodePatterns>{})
        .addPatterns(ClientPatternsAdder<HostageServicesPatterns>{})
        .addPatterns(ClientPatternsAdder<GlowSceneObjectPatterns>{})
        .addPatterns(ClientPatternsAdder<MemAllocPatterns>{})
        .addPatterns(ClientPatternsAdder<PanelPatterns>{})
        .addPatterns(ClientPatternsAdder<PanoramaImagePanelPatterns>{})
        .addPatterns(ClientPatternsAdder<PanoramaLabelPatterns>{})
        .finalize();
}();
