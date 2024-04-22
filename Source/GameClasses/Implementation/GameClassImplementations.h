#pragma once

#include "ConVarImpl.h"
#include "CvarImpl.h"
#include "EntityImpl.h"
#include "EntitySystemImpl.h"
#include "FileNameSymbolTableImpl.h"
#include "FileSystemImpl.h"
#include "GameRulesImpl.h"
#include "GameSceneNodeImpl.h"
#include "MemAllocImpl.h"
#include "PanelImpl.h"
#include "PanelStyleImpl.h"
#include "PanoramaImagePanelImpl.h"
#include "PanoramaLabelImpl.h"
#include "PanoramaUiEngineImpl.h"
#include "PanoramaUiPanelImpl.h"
#include "PlantedC4Impl.h"
#include "PlayerControllerImpl.h"
#include "PlayerPawnImpl.h"
#include "TopLevelWindowImpl.h"
#include "WeaponServicesImpl.h"
#include "WeaponVDataImpl.h"

#include <Platform/VmtFinder.h>

struct GameClassImplementations {
    GameClassImplementations(const PatternFinder<PatternNotFoundLogger>& clientPatternFinder,
                             const PatternFinder<PatternNotFoundLogger>& panoramaPatternFinder,
                             const PatternFinder<PatternNotFoundLogger>& tier0PatternFinder,
                             const FileSystemPatterns& fileSystemPatterns,
                             Tier0Dll tier0Dll) noexcept
        : conVar{ConVarPatterns{tier0PatternFinder}}
        , cvar{CvarPatterns{clientPatternFinder, tier0PatternFinder}}
        , entity{EntityPatterns{clientPatternFinder}}
        , entitySystem{EntitySystemPatterns{clientPatternFinder}}
        , fileNameSymbolTable{tier0Dll}
        , fileSystem{fileSystemPatterns}
        , gameRules{GameRulesPatterns{clientPatternFinder}}
        , gameSceneNode{GameSceneNodePatterns{clientPatternFinder}}
        , memAlloc{tier0Dll, MemAllocPatterns{clientPatternFinder}}
        , panel{PanelPatterns{clientPatternFinder}}
        , panelStyle{PanelStylePatterns{panoramaPatternFinder}}
        , imagePanel{PanoramaImagePanelPatterns{clientPatternFinder}}
        , panoramaLabel{PanoramaLabelPatterns{clientPatternFinder}}
        , uiEngine{ClientPatterns{clientPatternFinder}, PanoramaUiEnginePatterns{panoramaPatternFinder}}
        , panoramaUiPanelOffsets{PanoramaUiPanelPatterns{clientPatternFinder, panoramaPatternFinder}}
        , plantedC4{PlantedC4Patterns{clientPatternFinder}}
        , playerController{PlayerControllerPatterns{clientPatternFinder}}
        , playerPawn{PlayerPawnPatterns{clientPatternFinder}}
        , topLevelWindow{TopLevelWindowPatterns{panoramaPatternFinder}}
        , weaponServices{WeaponServicesPatterns{clientPatternFinder}}
        , weaponVData{WeaponVDataPatterns{clientPatternFinder}}
    {
    }

    ConVarImpl conVar;
    CvarImpl cvar;
    EntityImpl entity;
    EntitySystemImpl entitySystem;
    FileNameSymbolTableImpl fileNameSymbolTable;
    FileSystemImpl fileSystem;
    GameRulesImpl gameRules;
    GameSceneNodeImpl gameSceneNode;
    MemAllocImpl memAlloc;
    PanelImpl panel;
    PanelStyleImpl panelStyle;
    PanoramaImagePanelImpl imagePanel;
    PanoramaLabelImpl panoramaLabel;
    PanoramaUiEngineImpl uiEngine;
    PanoramaUiPanelImpl panoramaUiPanelOffsets;
    PlantedC4Impl plantedC4;
    PlayerControllerImpl playerController;
    PlayerPawnImpl playerPawn;
    TopLevelWindowImpl topLevelWindow;
    WeaponServicesImpl weaponServices;
    WeaponVDataImpl weaponVData;
};
