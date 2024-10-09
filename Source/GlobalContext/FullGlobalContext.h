#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <GameDependencies/GameDependencies.h>
#include <GameDLLs/Tier0Dll.h>
#include <FeatureHelpers/EntityClassifier.h>
#include <FeatureHelpers/FeatureHelpers.h>
#include <FeatureHelpers/RenderingHookEntityLoop.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <Features/Features.h>
#include <Features/FeaturesStates.h>
#include <Features/FeaturesUnloadHandler.h>
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
        : dummy{findPatterns(memoryPatterns)}
        , gameDependencies{
            memoryPatterns,
            VmtFinder{clientDLL.getVmtFinderParams()},
            VmtFinder{panoramaDLL.getVmtFinderParams()},
            Tier0Dll{}}
        , hooks{
            peepEventsHook,
            clientPatternSearchResults.get<ViewRenderPointer>(),
            VmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()}}
        , entityClassifier{gameDependencies.entitiesVMTs}
    {
    }

    [[nodiscard]] auto features(auto& dependencies) noexcept
    {
        return Features{featuresStates, featureHelpers, hooks, dependencies};
    }

    PatternSearchResults<decltype(kClientPatterns)> clientPatternSearchResults;
    bool dummy{};
    GameDependencies gameDependencies;
    Hooks hooks;
    FeatureHelpers featureHelpers;
    FeaturesStates featuresStates;
    PanoramaGuiState panoramaGuiState;
    BombStatusPanelState bombStatusPanelState;
    InWorldPanelContainerState inWorldPanelContainerState;
    GlowSceneObjectsState glowSceneObjectsState;
    EntityClassifier entityClassifier;

private:
    bool findPatterns(const MemoryPatterns& memoryPatterns) noexcept
    {
        memoryPatterns.patternFinders.clientPatternFinder.findPatterns(kClientPatterns, clientPatternSearchResults);
        return true;
    }
};
