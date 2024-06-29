#pragma once

#include "ConVars.h"
#include "ConVarDeps.h"
#include "CvarDeps.h"
#include "EntityDeps.h"
#include "EntitiesVMTs.h"
#include "EntitySystemDeps.h"
#include "FileNameSymbolTableDeps.h"
#include "FileSystemDeps.h"
#include "GameRulesDeps.h"
#include "GameSceneNodeDeps.h"
#include "GlobalVarsDeps.h"
#include "HostageServicesDeps.h"
#include "HudDeps.h"
#include "MemAllocDeps.h"
#include "PanelDeps.h"
#include "PanelStyleDeps.h"
#include "PanoramaImagePanelDeps.h"
#include "PanoramaLabelDeps.h"
#include "PanoramaSymbols.h"
#include "PanoramaUiEngineDeps.h"
#include "PanoramaUiPanelDeps.h"
#include "PlantedC4Deps.h"
#include "PlayerControllerDeps.h"
#include "PlayerPawnDeps.h"
#include "TopLevelWindowDeps.h"
#include "WeaponDeps.h"
#include "WeaponServicesDeps.h"
#include "WeaponVDataDeps.h"

#include <FeatureHelpers/MainMenuProvider.h>
#include <FeatureHelpers/PanelConfigurator.h>
#include <FeatureHelpers/StylePropertiesSymbolsAndVMTs.h>

#include <MemoryPatterns/MemoryPatterns.h>
#include <Platform/VmtFinder.h>

struct GameDependencies {
    GameDependencies(const MemoryPatterns& memoryPatterns, const VmtFinder& clientVmtFinder, const VmtFinder& panoramaVmtFinder, Tier0Dll tier0Dll) noexcept
        : conVarDeps{memoryPatterns.conVarPatterns()}
        , cvarDeps{memoryPatterns.cvarPatterns()}
        , entityDeps{memoryPatterns.entityPatterns()}
        , entitySystemDeps{memoryPatterns.entitySystemPatterns()}
        , fileNameSymbolTableDeps{tier0Dll}
        , fileSystemDeps{memoryPatterns.fileSystemPatterns()}
        , gameRulesDeps{memoryPatterns.gameRulesPatterns()}
        , gameSceneNodeDeps{memoryPatterns.gameSceneNodePatterns()}
        , globalVarsDeps{memoryPatterns.clientPatterns()}
        , hostageServicesDeps{memoryPatterns.hostageServicesPatterns()}
        , hudDeps{memoryPatterns.clientPatterns()}
        , memAllocDeps{tier0Dll, memoryPatterns.memAllocPatterns()}
        , panelDeps{memoryPatterns.panelPatterns()}
        , panelStyleDeps{memoryPatterns.panelStylePatterns()}
        , imagePanelDeps{memoryPatterns.panoramaImagePanelPatterns()}
        , panoramaLabelDeps{memoryPatterns.panoramaLabelPatterns()}
        , uiEngineDeps{memoryPatterns.panoramaUiEnginePatterns()}
        , panoramaUiPanelDeps{memoryPatterns.panoramaUiPanelPatterns()}
        , plantedC4Deps{memoryPatterns.plantedC4Patterns()}
        , playerControllerDeps{memoryPatterns.playerControllerPatterns()}
        , playerPawnDeps{memoryPatterns.playerPawnPatterns()}
        , topLevelWindowDeps{memoryPatterns.topLevelWindowPatterns()}
        , weaponDeps{memoryPatterns.weaponPatterns()}
        , weaponServicesDeps{memoryPatterns.weaponServicesPatterns()}
        , weaponVDataDeps{memoryPatterns.weaponVDataPatterns()}
        , loopModeGame{memoryPatterns.clientPatterns().loopModeGame()}
        , viewRender{memoryPatterns.clientPatterns().viewRender()}
        , transformTranslate3dVmt{memoryPatterns.clientPatterns().transformTranslate3dVMT()}
        , transformScale3dVmt{memoryPatterns.clientPatterns().transformScale3dVMT()}
        , worldToProjectionMatrix{memoryPatterns.clientPatterns().worldToProjectionMatrix()}
        , viewToProjectionMatrix{memoryPatterns.clientPatterns().viewToProjectionMatrix()}
        , stylePropertiesSymbolsAndVMTs{StylePropertySymbolMap{memoryPatterns.panelStylePatterns().stylePropertiesSymbols()}, panoramaVmtFinder}
        , mainMenuProvider{memoryPatterns.clientPatterns()}
        , localPlayerController{memoryPatterns.clientPatterns().localPlayerController()}
        , entitiesVMTs{clientVmtFinder}
        , soundChannels{memoryPatterns.soundSystemPatterns().soundChannels()}
        , fileSystem{memoryPatterns.fileSystemPatterns().fileSystem()}
    {
    }

    [[nodiscard]] PanelConfigurator panelConfigurator() const noexcept
    {
        return PanelConfigurator{PanelStylePropertyFactory{stylePropertiesSymbolsAndVMTs}};    
    }

    ConVarDeps conVarDeps;
    CvarDeps cvarDeps;
    EntityDeps entityDeps;
    EntitySystemDeps entitySystemDeps;
    FileNameSymbolTableDeps fileNameSymbolTableDeps;
    FileSystemDeps fileSystemDeps;
    GameRulesDeps gameRulesDeps;
    GameSceneNodeDeps gameSceneNodeDeps;
    GlobalVarsDeps globalVarsDeps;
    HostageServicesDeps hostageServicesDeps;
    HudDeps hudDeps;
    MemAllocDeps memAllocDeps;
    PanelDeps panelDeps;
    PanelStyleDeps panelStyleDeps;
    PanoramaImagePanelDeps imagePanelDeps;
    PanoramaLabelDeps panoramaLabelDeps;
    PanoramaUiEngineDeps uiEngineDeps;
    PanoramaUiPanelDeps panoramaUiPanelDeps;
    PlantedC4Deps plantedC4Deps;
    PlayerControllerDeps playerControllerDeps;
    PlayerPawnDeps playerPawnDeps;
    TopLevelWindowDeps topLevelWindowDeps;
    WeaponDeps weaponDeps;
    WeaponServicesDeps weaponServicesDeps;
    WeaponVDataDeps weaponVDataDeps;

    cs2::CLoopModeGame** loopModeGame;
    cs2::CViewRender** viewRender;
    const void* transformTranslate3dVmt; // todo: create struct for transform VMTs
    const void* transformScale3dVmt;
    const cs2::VMatrix* worldToProjectionMatrix;
    const cs2::VMatrix* viewToProjectionMatrix;
    StylePropertiesSymbolsAndVMTs stylePropertiesSymbolsAndVMTs;
    MainMenuProvider mainMenuProvider; // only needed for GUI initialization
    cs2::CCSPlayerController** localPlayerController;
    EntitiesVMTs entitiesVMTs;
    cs2::SoundChannels** soundChannels;
    cs2::CBaseFileSystem** fileSystem;
    std::optional<ConVars> conVars;
    std::optional<PanoramaSymbols> panoramaSymbols;
};
