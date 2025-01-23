#pragma once

#if IS_WIN64()
#include "Windows/WindowsPatterns.h"
#elif IS_LINUX()
#include "Linux/LinuxPatterns.h"
#endif

#include "PatternFinders.h"

#include <MemorySearch/PatternPool.h>
#include <MemorySearch/TempPatternPool.h>

struct MemoryPatterns {
    const PatternFinders& patternFinders;

#define MEMORY_PATTERNS(type, name) \
[[nodiscard]] auto name() const noexcept \
{ \
    return type<PatternFinders>{patternFinders}; \
}

    MEMORY_PATTERNS(PanelStylePatterns, panelStylePatterns)
    MEMORY_PATTERNS(SdlPatterns, sdlPatterns)

#undef MEMORY_PATTERNS
};

constexpr auto kClientPatterns = []() consteval {
#define ADD_PATTERNS(patterns) addPatterns([](auto patternPool) consteval { return patterns::addClientPatterns(patternPool); })
    constexpr auto builder = PatternPoolBuilder<TempPatternPool<1500, 100>>{}
        .ADD_PATTERNS(BaseModelEntityPatterns)
        .ADD_PATTERNS(C4Patterns)
        .ADD_PATTERNS(ClientPatterns)
        .ADD_PATTERNS(CvarPatterns)
        .ADD_PATTERNS(EntityPatterns)
        .ADD_PATTERNS(EntitySystemPatterns)
        .ADD_PATTERNS(GameRulesPatterns)
        .ADD_PATTERNS(GameSceneNodePatterns)
        .ADD_PATTERNS(HostageServicesPatterns)
        .ADD_PATTERNS(GlowPropertyPatterns)
        .ADD_PATTERNS(GlowSceneObjectPatterns)
        .ADD_PATTERNS(MapPlayerPreviewPanelPatterns)
        .ADD_PATTERNS(MemAllocPatterns)
        .ADD_PATTERNS(PanelPatterns)
        .ADD_PATTERNS(PanoramaDropDownPatterns)
        .ADD_PATTERNS(PanoramaImagePanelPatterns)
        .ADD_PATTERNS(PanoramaLabelPatterns)
        .ADD_PATTERNS(PanoramaUiEnginePatterns)
        .ADD_PATTERNS(PanoramaUiPanelPatterns)
        .ADD_PATTERNS(PlantedC4Patterns)
        .ADD_PATTERNS(PlayerControllerPatterns)
        .ADD_PATTERNS(PlayerPawnPatterns)
        .ADD_PATTERNS(RenderComponentPatterns)
        .ADD_PATTERNS(SceneObjectUpdaterPatterns)
        .ADD_PATTERNS(SmokeGrenadeProjectilePatterns)
        .ADD_PATTERNS(WeaponPatterns)
        .ADD_PATTERNS(WeaponServicesPatterns)
        .ADD_PATTERNS(WeaponVDataPatterns);
#undef ADD_PATTERNS
    return PatternPool<>::from<builder>();
}();

constexpr auto kSceneSystemPatterns = []() consteval {
#define ADD_PATTERNS(patterns) addPatterns([](auto patternPool) consteval { return patterns::addSceneSystemPatterns(patternPool); })
    constexpr auto builder = PatternPoolBuilder<TempPatternPool<1500, 100>>{}
        .ADD_PATTERNS(SceneObjectPatterns)
        .ADD_PATTERNS(SceneSystemPatterns);
#undef ADD_PATTERNS
    return PatternPool<>::from<builder>();
}();

constexpr auto kTier0Patterns = []() consteval {
#define ADD_PATTERNS(patterns) addPatterns([](auto patternPool) consteval { return patterns::addTier0Patterns(patternPool); })
    constexpr auto builder = PatternPoolBuilder<TempPatternPool<1500, 100>>{}
        .ADD_PATTERNS(ConVarPatterns)
        .ADD_PATTERNS(CvarPatterns);
#undef ADD_PATTERNS
    return PatternPool<>::from<builder>();
}();

constexpr auto kFileSystemPatterns = []() consteval {
#define ADD_PATTERNS(patterns) addPatterns([](auto patternPool) consteval { return patterns::addFileSystemPatterns(patternPool); })
    constexpr auto builder = PatternPoolBuilder<TempPatternPool<1500, 100>>{}
        .ADD_PATTERNS(FileSystemPatterns);
#undef ADD_PATTERNS
    return PatternPool<>::from<builder>();
}();

constexpr auto kSoundSystemPatterns = []() consteval {
#define ADD_PATTERNS(patterns) addPatterns([](auto patternPool) consteval { return patterns::addSoundSystemPatterns(patternPool); })
    constexpr auto builder = PatternPoolBuilder<TempPatternPool<1500, 100>>{}
        .ADD_PATTERNS(FileSystemPatterns)
        .ADD_PATTERNS(SoundSystemPatterns);
#undef ADD_PATTERNS
    return PatternPool<>::from<builder>();
}();

constexpr auto kPanoramaPatterns = []() consteval {
#define ADD_PATTERNS(patterns) addPatterns([](auto patternPool) consteval { return patterns::addPanoramaPatterns(patternPool); })
    constexpr auto builder = PatternPoolBuilder<TempPatternPool<1500, 100>>{}
        .ADD_PATTERNS(PanelStylePatterns2)
        .ADD_PATTERNS(PanoramaUiEnginePatterns)
        .ADD_PATTERNS(PanoramaUiPanelPatterns)
        .ADD_PATTERNS(TopLevelWindowPatterns);
#undef ADD_PATTERNS
    return PatternPool<>::from<builder>();
}();
