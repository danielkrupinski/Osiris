#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <GameClasses/Implementation/GameClassImplementations.h>
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
        : _gameClasses{
            memoryPatterns,
            Tier0Dll{}}
        , hooks{
            peepEventsHook,
            memoryPatterns.clientPatterns(),
            VmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()}}
        , featureHelpers{
            memoryPatterns,
            VmtFinder{panoramaDLL.getVmtFinderParams()},
            VmtFinder{clientDLL.getVmtFinderParams()}}
    {
    }

    [[nodiscard]] const GameClassImplementations& gameClasses() const noexcept
    {
        return _gameClasses;
    }

    [[nodiscard]] const FeatureHelpers& getFeatureHelpers() const noexcept
    {
        return featureHelpers;
    }
    
    void onRenderStart(cs2::CViewRender* thisptr) noexcept
    {
        hooks.viewRenderHook.getOriginalOnRenderStart()(thisptr);

        HookDependencies dependencies{_gameClasses, featureHelpers};
        SoundWatcher soundWatcher{featureHelpers.soundWatcherState, dependencies};
        soundWatcher.update();
        features(dependencies).soundFeatures().runOnViewMatrixUpdate();

        PlayerInformationThroughWalls playerInformationThroughWalls{featuresStates.visualFeaturesStates.playerInformationThroughWallsState, dependencies};
        RenderingHookEntityLoop{dependencies, playerInformationThroughWalls}.run();
        playerInformationThroughWalls.hideUnusedPanels();
    }

    [[nodiscard]] PeepEventsHookResult onPeepEventsHook() noexcept
    {
        HookDependencies dependencies{_gameClasses, featureHelpers};

        features(dependencies).hudFeatures().bombTimer().run();
        features(dependencies).hudFeatures().defusingAlert().run();
        features(dependencies).hudFeatures().killfeedPreserver().run();

        UnloadFlag unloadFlag;
        panoramaGUI.run(features(dependencies), unloadFlag);
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

    GameClassImplementations _gameClasses;
    Hooks hooks;
    FeatureHelpers featureHelpers;
    FeaturesStates featuresStates;
public:
    PanoramaGUI panoramaGUI;
};
