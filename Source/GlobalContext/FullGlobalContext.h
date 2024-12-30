#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <GameDependencies/GameDependencies.h>
#include <GameDLLs/Tier0Dll.h>
#include <FeatureHelpers/EntityClassifier.h>
#include <FeatureHelpers/RenderingHookEntityLoop.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <Features/Common/InWorldPanelsState.h>
#include <Features/Features.h>
#include <Features/FeaturesStates.h>
#include <Features/FeaturesUnloadHandler.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheState.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <Helpers/UnloadFlag.h>
#include <Hooks/Hooks.h>
#include <Hooks/PeepEventsHook.h>
#include <Hud/BombStatus/BombStatusPanelManager.h>
#include <Hud/BombStatus/BombStatusPanelManagerContext.h>
#include <Hud/BombStatus/BombStatusPanelState.h>
#include <Hud/BombStatus/BombStatusPanelUnloadHandler.h>
#include <MemorySearch/PatternFinder.h>
#include <MemorySearch/PatternSearchResults.h>
#include <UI/Panorama/PanoramaGUI.h>
#include <UI/Panorama/PanoramaGuiState.h>
#include <UI/Panorama/PanoramaGuiUnloadHandler.h>
#include <OutlineGlow/GlowSceneObjectsState.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/VmtFinder.h>
#include <Vmt/VmtLengthCalculator.h>

#include "PeepEventsHookResult.h"

#include <CS2/Classes/ConVarTypes.h>

struct FullGlobalContext {
    FullGlobalContext(PeepEventsHook peepEventsHook, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, const MemoryPatterns& memoryPatterns) noexcept
        : clientPatternSearchResults{memoryPatterns.patternFinders.clientPatternFinder.findPatterns(kClientPatterns)}
        , sceneSystemPatternSearchResults{memoryPatterns.patternFinders.sceneSystemPatternFinder.findPatterns(kSceneSystemPatterns)}
        , tier0PatternSearchResults{memoryPatterns.patternFinders.tier0PatternFinder.findPatterns(kTier0Patterns)}
        , fileSystemPatternSearchResults{memoryPatterns.patternFinders.fileSystemPatternFinder.findPatterns(kFileSystemPatterns)}
        , soundSystemPatternSearchResults{memoryPatterns.patternFinders.soundSystemPatternFinder.findPatterns(kSoundSystemPatterns)}
        , panoramaPatternSearchResults{memoryPatterns.patternFinders.panoramaPatternFinder.findPatterns(kPanoramaPatterns)}
        , gameDependencies{
            memoryPatterns,
            VmtFinder{panoramaDLL.getVmtFinderParams()},
            Tier0Dll{}}
        , hooks{
            peepEventsHook,
            clientPatternSearchResults.get<ViewRenderPointer>(),
            VmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()}}
    {
    }

    [[nodiscard]] auto features(auto& dependencies) noexcept
    {
        return Features{featuresStates, hooks, dependencies};
    }

    PatternSearchResults<decltype(kClientPatterns)> clientPatternSearchResults;
    PatternSearchResults<decltype(kSceneSystemPatterns)> sceneSystemPatternSearchResults;
    PatternSearchResults<decltype(kTier0Patterns)> tier0PatternSearchResults;
    PatternSearchResults<decltype(kFileSystemPatterns)> fileSystemPatternSearchResults;
    PatternSearchResults<decltype(kSoundSystemPatterns)> soundSystemPatternSearchResults;
    PatternSearchResults<decltype(kPanoramaPatterns)> panoramaPatternSearchResults;
    GameDependencies gameDependencies;
    Hooks hooks;
    SoundWatcherState soundWatcherState;
    FeaturesStates featuresStates;
    PanoramaGuiState panoramaGuiState;
    BombStatusPanelState bombStatusPanelState;
    InWorldPanelsState inWorldPanelsState;
    GlowSceneObjectsState glowSceneObjectsState;
    EntityClassifier entityClassifier;
    PlayerInfoPanelCacheState playerInfoPanelCacheState;
};
