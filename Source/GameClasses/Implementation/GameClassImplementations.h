#pragma once

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

#include <Platform/VmtFinder.h>

struct GameClassImplementations {
    GameClassImplementations(const ClientPatterns& clientPatterns,
                             const FileSystemPatterns& fileSystemPatterns,
                             const GameRulesPatterns& gameRulesPatterns,
                             const MemAllocPatterns& memAllocPatterns,
                             const PanelPatterns& panelPatterns,
                             const PanelStylePatterns& panelStylePatterns,
                             const PanoramaImagePanelPatterns& panoramaImagePanelPatterns,
                             const PanoramaLabelPatterns& panoramaLabelPatterns,
                             const PanoramaUiEnginePatterns& panoramaUiEnginePatterns,
                             const PanoramaUiPanelPatterns& panoramaUiPanelPatterns,
                             const PlantedC4Patterns& plantedC4Patterns,
                             const EntitySystemPatterns& entitySystemPatterns,
                             const PlayerControllerPatterns& playerControllerPatterns,
                             const TopLevelWindowPatterns& topLevelWindowPatterns,
                             const EntityPatterns& entityPatterns,
                             const GameSceneNodePatterns& gameSceneNodePatterns,
                             const PlayerPawnPatterns& playerPawnPatterns,
                             Tier0Dll tier0Dll) noexcept
        : entity{entityPatterns}
        , entitySystem{entitySystemPatterns}
        , fileNameSymbolTable{tier0Dll}
        , fileSystem{fileSystemPatterns}
        , gameRules{gameRulesPatterns}
        , gameSceneNode{gameSceneNodePatterns}
        , memAlloc{tier0Dll, memAllocPatterns}
        , panel{panelPatterns}
        , panelStyle{panelStylePatterns}
        , imagePanel{panoramaImagePanelPatterns}
        , panoramaLabel{panoramaLabelPatterns}
        , uiEngine{clientPatterns, panoramaUiEnginePatterns}
        , panoramaUiPanelOffsets{panoramaUiPanelPatterns}
        , plantedC4{plantedC4Patterns}
        , playerController{playerControllerPatterns}
        , playerPawn{playerPawnPatterns}
        , topLevelWindow{topLevelWindowPatterns}
    {
    }

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
};
