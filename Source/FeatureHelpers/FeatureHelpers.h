#pragma once

#include "ConVars.h"
#include "EntitiesVMTs.h"
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include "HudInWorldPanelContainer.h"
#include "MainMenuProvider.h"
#include <MemoryPatterns/SoundSystemPatterns.h>
#include "PanelConfigurator.h"
#include "Sound/SoundWatcherState.h"
#include "StylePropertiesSymbolsAndVMTs.h"
#include "StylePropertySymbolMap.h"
#include "ViewToProjectionMatrix.h"
#include "WorldToClipSpaceConverter.h"

struct FeatureHelpers {
    explicit FeatureHelpers(const ClientPatterns& clientPatterns, const PanelStylePatterns& panelStylePatterns, const FileSystemPatterns& fileSystemPatterns, const SoundSystemPatterns& soundSystemPatterns, const VmtFinder& panoramaVmtFinder, const VmtFinder& clientVmtFinder) noexcept
        : hudProvider{clientPatterns}
        , globalVars{clientPatterns.globalVars()}
        , transformFactory{clientPatterns}
        , worldtoClipSpaceConverter{clientPatterns}
        , viewToProjectionMatrix{clientPatterns}
        , stylePropertiesSymbolsAndVMTs{StylePropertySymbolMap{panelStylePatterns.stylePropertiesSymbols()}, panoramaVmtFinder}
        , gameRules{clientPatterns.gameRules()}
        , mainMenuProvider{clientPatterns}
        , localPlayerController{clientPatterns.localPlayerController()}
        , entitiesVMTs{clientVmtFinder}
        , plantedC4s{clientPatterns.plantedC4s()}
        , soundChannels{soundSystemPatterns.soundChannels()}
        , fileSystem{fileSystemPatterns.fileSystem()}
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
    cs2::C_CSGameRules** gameRules;
    MainMenuProvider mainMenuProvider;
    cs2::CCSPlayerController** localPlayerController;
    EntitiesVMTs entitiesVMTs;
    cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s;
    cs2::SoundChannels** soundChannels;
    cs2::CBaseFileSystem** fileSystem;
    std::optional<ConVars> conVars;
};
