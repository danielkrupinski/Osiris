#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <GameDependencies/GameDependencies.h>
#include <GameDLLs/Tier0Dll.h>
#include <FeatureHelpers/FeatureHelpers.h>
#include <FeatureHelpers/RenderingHookEntityLoop.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <Features/Features.h>
#include <Features/FeaturesStates.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <Helpers/UnloadFlag.h>
#include <Hooks/Hooks.h>
#include <Hooks/PeepEventsHook.h>
#include <MemorySearch/PatternFinder.h>
#include <UI/Panorama/PanoramaGUI.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/VmtFinder.h>
#include <Vmt/VmtLengthCalculator.h>

#include "PeepEventsHookResult.h"

#include <CS2/Classes/ConVarTypes.h>

struct FullGlobalContext {
    FullGlobalContext(PeepEventsHook peepEventsHook, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, const MemoryPatterns& memoryPatterns) noexcept
        : _gameDependencies{
            memoryPatterns,
            VmtFinder{clientDLL.getVmtFinderParams()},
            VmtFinder{panoramaDLL.getVmtFinderParams()},
            Tier0Dll{}}
        , hooks{
            peepEventsHook,
            _gameDependencies.loopModeGame,
            _gameDependencies.viewRender,
            VmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()}}
    {
    }

    [[nodiscard]] GameDependencies& gameDependencies() noexcept
    {
        return _gameDependencies;
    }

    [[nodiscard]] FeatureHelpers& getFeatureHelpers() noexcept
    {
        return featureHelpers;
    }
    
    void onRenderStart(cs2::CViewRender* thisptr) noexcept
    {
        hooks.viewRenderHook.getOriginalOnRenderStart()(thisptr);

        HookDependencies dependencies{_gameDependencies, featureHelpers};
        SoundWatcher soundWatcher{featureHelpers.soundWatcherState, dependencies};
        soundWatcher.update();
        features(dependencies).soundFeatures().runOnViewMatrixUpdate();

        PlayerInformationThroughWalls playerInformationThroughWalls{featuresStates.visualFeaturesStates.playerInformationThroughWallsState, dependencies};
        RenderingHookEntityLoop{dependencies, playerInformationThroughWalls}.run();
        playerInformationThroughWalls.hideUnusedPanels();
    }

    [[nodiscard]] PeepEventsHookResult onPeepEventsHook() noexcept
    {
        HookDependencies dependencies{_gameDependencies, featureHelpers};

        features(dependencies).hudFeatures().bombTimer().run();
        features(dependencies).hudFeatures().defusingAlert().run();
        features(dependencies).hudFeatures().killfeedPreserver().run();

        UnloadFlag unloadFlag;
        panoramaGUI.run(dependencies, features(dependencies), unloadFlag);
        hooks.update();

        if (unloadFlag)
            hooks.forceUninstall();

        return PeepEventsHookResult{hooks.peepEventsHook.original, static_cast<bool>(unloadFlag)};
    }

    [[nodiscard]] cs2::CLoopModeGame::getWorldSession getWorldSessionHook(ReturnAddress) noexcept
    {
        return hooks.loopModeGameHook.originalGetWorldSession;
    }

private:
    [[nodiscard]] Features features(HookDependencies& dependencies) noexcept
    {
        return Features{featuresStates, featureHelpers, hooks, dependencies};
    }

    GameDependencies _gameDependencies;
    Hooks hooks;
    FeatureHelpers featureHelpers;
    FeaturesStates featuresStates;
public:
    PanoramaGUI panoramaGUI;
};
