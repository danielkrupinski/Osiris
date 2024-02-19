#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <GameClasses/Implementation/GameClassImplementations.h>
#include <GameDLLs/Tier0Dll.h>
#include <FeatureHelpers/FeatureHelpers.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <Features/Features.h>
#include <Features/FeaturesStates.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <Helpers/UnloadFlag.h>
#include <Hooks/Hooks.h>
#include <Hooks/PeepEventsHook.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <MemoryPatterns/FileSystemPatterns.h>
#include <MemoryPatterns/GameRulesPatterns.h>
#include <MemoryPatterns/MemAllocPatterns.h>
#include <MemoryPatterns/PanelPatterns.h>
#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemoryPatterns/PanoramaLabelPatterns.h>
#include <MemoryPatterns/PanoramaUiEnginePatterns.h>
#include <MemoryPatterns/PanoramaUiPanelPatterns.h>
#include <MemoryPatterns/PlantedC4Patterns.h>
#include <MemoryPatterns/SoundSystemPatterns.h>
#include <MemoryPatterns/TopLevelWindowPatterns.h>
#include <MemorySearch/PatternFinder.h>
#include <UI/Panorama/PanoramaGUI.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/VmtFinder.h>
#include <Vmt/VmtLengthCalculator.h>

#include "PeepEventsHookResult.h"

struct FullGlobalContext {
    FullGlobalContext(PeepEventsHook peepEventsHook, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, const PatternFinder<PatternNotFoundLogger>& clientPatternFinder, const PatternFinder<PatternNotFoundLogger>& panoramaPatternFinder, const PatternFinder<PatternNotFoundLogger>& soundSystemPatternFinder, const FileSystemPatterns& fileSystemPatterns) noexcept
        : _gameClasses{
            ClientPatterns{clientPatternFinder},
            fileSystemPatterns,
            GameRulesPatterns{clientPatternFinder},
            MemAllocPatterns{clientPatternFinder},
            PanelPatterns{clientPatternFinder},
            PanelStylePatterns{panoramaPatternFinder},
            PanoramaImagePanelPatterns{clientPatternFinder},
            PanoramaLabelPatterns{clientPatternFinder},
            PanoramaUiEnginePatterns{panoramaPatternFinder},
            PanoramaUiPanelPatterns{clientPatternFinder, panoramaPatternFinder},
            PlantedC4Patterns{clientPatternFinder},
            EntitySystemPatterns{clientPatternFinder},
            TopLevelWindowPatterns{panoramaPatternFinder},
            Tier0Dll{}}
        , hooks{
            peepEventsHook,
            ClientPatterns{clientPatternFinder},
            VmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()}}
        , featureHelpers{
            ClientPatterns{clientPatternFinder},
            PanelStylePatterns{panoramaPatternFinder},
            fileSystemPatterns,
            SoundSystemPatterns{soundSystemPatternFinder},
            VmtFinder{panoramaDLL.getVmtFinderParams()}}
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
        if (featureHelpers.globalVarsProvider && featureHelpers.globalVarsProvider.getGlobalVars())
            featureHelpers.soundWatcher.update(featureHelpers.globalVarsProvider.getGlobalVars()->curtime);
        features().soundFeatures().runOnViewMatrixUpdate();
    }

    [[nodiscard]] PeepEventsHookResult onPeepEventsHook() noexcept
    {
        features().hudFeatures().bombTimer().run();
        features().hudFeatures().defusingAlert().run();
        features().hudFeatures().killfeedPreserver().run();

        UnloadFlag unloadFlag;
        panoramaGUI.run(features(), unloadFlag);
        hooks.update();
        features().visualFeatures().scopeOverlayRemover().updatePanelVisibility();

        if (unloadFlag)
            hooks.forceUninstall();

        return PeepEventsHookResult{hooks.peepEventsHook.original, static_cast<bool>(unloadFlag)};
    }

    [[nodiscard]] cs2::CLoopModeGame::getWorldSession getWorldSessionHook(ReturnAddress) noexcept
    {
        return hooks.loopModeGameHook.originalGetWorldSession;
    }

private:
    [[nodiscard]] Features features() noexcept
    {
        return Features{featuresStates, featureHelpers, hooks};
    }

    GameClassImplementations _gameClasses;
    Hooks hooks;
    FeatureHelpers featureHelpers;
    FeaturesStates featuresStates;
public:
    PanoramaGUI panoramaGUI;
};
