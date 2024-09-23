#pragma once

#if IS_WIN64()
#include "Windows/WindowsPatterns.h"
#elif IS_LINUX()
#include "Linux/LinuxPatterns.h"
#endif

#include "PatternFinders.h"

struct MemoryPatterns {
    const PatternFinders& patternFinders;

#define MEMORY_PATTERNS(type, name) \
[[nodiscard]] auto name() const noexcept \
{ \
    return type<PatternFinders>{patternFinders}; \
}

    MEMORY_PATTERNS(ClientPatterns, clientPatterns)
    MEMORY_PATTERNS(ConVarPatterns, conVarPatterns)
    MEMORY_PATTERNS(CvarPatterns, cvarPatterns)
    MEMORY_PATTERNS(EntityPatterns, entityPatterns)
    MEMORY_PATTERNS(EntitySystemPatterns, entitySystemPatterns)
    MEMORY_PATTERNS(FileSystemPatterns, fileSystemPatterns)
    MEMORY_PATTERNS(GameRulesPatterns, gameRulesPatterns)
    MEMORY_PATTERNS(GameSceneNodePatterns, gameSceneNodePatterns)
    MEMORY_PATTERNS(GlowSceneObjectPatterns, glowSceneObjectPatterns)
    MEMORY_PATTERNS(HostageServicesPatterns, hostageServicesPatterns)
    MEMORY_PATTERNS(MemAllocPatterns, memAllocPatterns)
    MEMORY_PATTERNS(PanelPatterns, panelPatterns)
    MEMORY_PATTERNS(PanelStylePatterns, panelStylePatterns)
    MEMORY_PATTERNS(PanoramaImagePanelPatterns, panoramaImagePanelPatterns)
    MEMORY_PATTERNS(PanoramaLabelPatterns, panoramaLabelPatterns)
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
