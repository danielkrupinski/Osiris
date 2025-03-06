#pragma once

#include <Config/ConfigState.h>
#include <CS2/Classes/CLoopModeGame.h>
#include <GameClient/ConVars/ConVars.h>
#include <GameClient/FileNameSymbolTableState.h>
#include <GameClient/Hud/HudState.h>
#include <GameClient/MemAllocState.h>
#include <GameClient/Panorama/PanoramaSymbols.h>
#include <OutlineGlow/GlowSceneObjectState.h>
#include <GameClient/DLLs/Tier0Dll.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Common/RenderingHookEntityLoop.h>
#include <Features/Sound/Details/SoundWatcher.h>
#include <Features/Common/InWorldPanelsState.h>
#include <Features/Features.h>
#include <Features/FeaturesStates.h>
#include <Features/FeaturesUnloadHandler.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreviewState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheState.h>
#include <MemoryPatterns/MemoryPatterns.h>
#include <MemorySearch/PatternNotFoundLogger.h>
#include "UnloadFlag.h"
#include <Hooks/Hooks.h>
#include <Hooks/PeepEventsHook.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelManager.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelManagerContext.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelState.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelUnloadHandler.h>
#include <MemorySearch/PatternFinder.h>
#include <MemorySearch/PatternSearchResults.h>
#include <UI/Panorama/PanoramaGUI.h>
#include <UI/Panorama/PanoramaGuiState.h>
#include <UI/Panorama/PanoramaGuiUnloadHandler.h>
#include <OutlineGlow/GlowSceneObjectsState.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/VmtFinder.h>
#include <Vmt/VmtLengthCalculator.h>

#include "OsirisDirectoryPath.h"
#include "PeepEventsHookResult.h"

#include <CS2/Classes/ConVarTypes.h>

struct FullGlobalContext {
    FullGlobalContext(PeepEventsHook peepEventsHook, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, const MemoryPatterns& memoryPatterns, Tier0Dll tier0Dll) noexcept
        : clientPatternSearchResults{memoryPatterns.patternFinders.clientPatternFinder.findPatterns(kClientPatterns)}
        , sceneSystemPatternSearchResults{memoryPatterns.patternFinders.sceneSystemPatternFinder.findPatterns(kSceneSystemPatterns)}
        , tier0PatternSearchResults{memoryPatterns.patternFinders.tier0PatternFinder.findPatterns(kTier0Patterns)}
        , fileSystemPatternSearchResults{memoryPatterns.patternFinders.fileSystemPatternFinder.findPatterns(kFileSystemPatterns)}
        , soundSystemPatternSearchResults{memoryPatterns.patternFinders.soundSystemPatternFinder.findPatterns(kSoundSystemPatterns)}
        , panoramaPatternSearchResults{memoryPatterns.patternFinders.panoramaPatternFinder.findPatterns(kPanoramaPatterns)}
        , fileNameSymbolTableState{tier0Dll}
        , memAllocState{tier0Dll}
        , stylePropertySymbolsAndVMTs{StylePropertySymbolMap{memoryPatterns.panelStylePatterns().stylePropertiesSymbols()}, VmtFinder{panoramaDLL.getVmtFinderParams()}}
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

    OsirisDirectoryPath osirisDirectoryPath;
    ConfigState configState;
    PatternSearchResults<decltype(kClientPatterns)> clientPatternSearchResults;
    PatternSearchResults<decltype(kSceneSystemPatterns)> sceneSystemPatternSearchResults;
    PatternSearchResults<decltype(kTier0Patterns)> tier0PatternSearchResults;
    PatternSearchResults<decltype(kFileSystemPatterns)> fileSystemPatternSearchResults;
    PatternSearchResults<decltype(kSoundSystemPatterns)> soundSystemPatternSearchResults;
    PatternSearchResults<decltype(kPanoramaPatterns)> panoramaPatternSearchResults;
    FileNameSymbolTableState fileNameSymbolTableState;
    GlowSceneObjectState glowSceneObjectState;
    HudState hudState;
    MemAllocState memAllocState;
    StylePropertiesSymbolsAndVMTs stylePropertySymbolsAndVMTs;
    std::optional<ConVars> conVars;
    std::optional<PanoramaSymbols> panoramaSymbols;
    Hooks hooks;
    SoundWatcherState soundWatcherState;
    FeaturesStates featuresStates;
    PanoramaGuiState panoramaGuiState;
    BombStatusPanelState bombStatusPanelState;
    InWorldPanelsState inWorldPanelsState;
    GlowSceneObjectsState glowSceneObjectsState;
    EntityClassifier entityClassifier;
    PlayerInfoPanelCacheState playerInfoPanelCacheState;
    PlayerModelGlowPreviewState playerModelGlowPreviewState;
};
