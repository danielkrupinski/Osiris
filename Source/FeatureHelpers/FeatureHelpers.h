#pragma once

#include "GlobalVarsProvider.h"
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Helpers/PlantedC4Provider.h>
#include "HudInWorldPanelContainer.h"
#include "MainMenuProvider.h"
#include "PanelConfigurator.h"
#include "Sound/SoundWatcher.h"
#include "StylePropertiesSymbols.h"
#include "StylePropertiesVMTs.h"
#include "StylePropertySymbolMap.h"
#include "ViewToProjectionMatrix.h"
#include "WorldToClipSpaceConverter.h"

struct FeatureHelpers {
    explicit FeatureHelpers(const ClientPatterns& clientPatterns, const PanelStylePatterns& panelStylePatterns, const FileSystemPatterns& fileSystemPatterns, const SoundSystemPatterns& soundSystemPatterns, const VmtFinder& panoramaVmtFinder) noexcept
        : hudProvider{clientPatterns}
        , globalVarsProvider{clientPatterns}
        , transformFactory{clientPatterns}
        , worldtoClipSpaceConverter{clientPatterns}
        , plantedC4Provider{clientPatterns}
        , viewToProjectionMatrix{clientPatterns}
        , stylePropertiesVMTs{panoramaVmtFinder}
        , stylePropertiesSymbols{StylePropertySymbolMap{panelStylePatterns.stylePropertiesSymbols()}}
        , soundWatcher{fileSystemPatterns, soundSystemPatterns}
        , gameRules{clientPatterns.gameRules()}
        , hudScope{clientPatterns.hudScope()}
        , mainMenuProvider{clientPatterns}
    {
    }

    [[nodiscard]] PanelConfigurator panelConfigurator() const noexcept
    {
        return PanelConfigurator{PanelStylePropertyFactory{stylePropertiesVMTs, stylePropertiesSymbols}};    
    }

    HudProvider hudProvider;
    GlobalVarsProvider globalVarsProvider;
    PanoramaTransformFactory transformFactory;
    WorldToClipSpaceConverter worldtoClipSpaceConverter;
    PlantedC4Provider plantedC4Provider;
    HudInWorldPanelContainer hudInWorldPanelContainer;
    ViewToProjectionMatrix viewToProjectionMatrix;
    StylePropertiesVMTs stylePropertiesVMTs;
    StylePropertiesSymbols stylePropertiesSymbols;
    SoundWatcher soundWatcher;
    cs2::C_CSGameRules** gameRules;
    cs2::CPanel2D** hudScope;
    MainMenuProvider mainMenuProvider;
};
