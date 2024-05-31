#pragma once

#include "ConVars.h"
#include "EntitiesVMTs.h"
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include "HudInWorldPanelContainer.h"
#include "MainMenuProvider.h"
#include "PanelConfigurator.h"
#include "Sound/SoundWatcherState.h"
#include "StylePropertiesSymbolsAndVMTs.h"
#include "StylePropertySymbolMap.h"
#include "ViewToProjectionMatrix.h"
#include "WorldToClipSpaceConverter.h"

struct FeatureHelpers {
    explicit FeatureHelpers(const MemoryPatterns& memoryPatterns, const VmtFinder& panoramaVmtFinder, const VmtFinder& clientVmtFinder) noexcept
        : hudProvider{memoryPatterns.clientPatterns()}
        , globalVars{memoryPatterns.clientPatterns().globalVars()}
        , transformFactory{memoryPatterns.clientPatterns()}
        , worldtoClipSpaceConverter{memoryPatterns.clientPatterns()}
        , viewToProjectionMatrix{memoryPatterns.clientPatterns()}
        , stylePropertiesSymbolsAndVMTs{StylePropertySymbolMap{memoryPatterns.panelStylePatterns().stylePropertiesSymbols()}, panoramaVmtFinder}
        , mainMenuProvider{memoryPatterns.clientPatterns()}
        , localPlayerController{memoryPatterns.clientPatterns().localPlayerController()}
        , entitiesVMTs{clientVmtFinder}
        , plantedC4s{memoryPatterns.clientPatterns().plantedC4s()}
        , soundChannels{memoryPatterns.soundSystemPatterns().soundChannels()}
        , fileSystem{memoryPatterns.fileSystemPatterns().fileSystem()}
    {
    }

    [[nodiscard]] PanelConfigurator panelConfigurator() const noexcept
    {
        return PanelConfigurator{PanelStylePropertyFactory{stylePropertiesSymbolsAndVMTs}};    
    }

    HudProvider hudProvider;
    cs2::GlobalVars** globalVars;
    PanoramaTransformFactory transformFactory;
    WorldToClipSpaceConverter worldtoClipSpaceConverter;
    HudInWorldPanelContainer hudInWorldPanelContainer;
    ViewToProjectionMatrix viewToProjectionMatrix;
    StylePropertiesSymbolsAndVMTs stylePropertiesSymbolsAndVMTs;
    SoundWatcherState soundWatcherState;
    MainMenuProvider mainMenuProvider;
    cs2::CCSPlayerController** localPlayerController;
    EntitiesVMTs entitiesVMTs;
    cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s;
    cs2::SoundChannels** soundChannels;
    cs2::CBaseFileSystem** fileSystem;
    std::optional<ConVars> conVars;
};
