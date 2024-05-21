#pragma once

#include "ConVarImpl.h"
#include "CvarImpl.h"
#include "EntityImpl.h"
#include "EntitySystemImpl.h"
#include "FileNameSymbolTableImpl.h"
#include "FileSystemImpl.h"
#include "GameRulesImpl.h"
#include "GameSceneNodeImpl.h"
#include "HostageServicesImpl.h"
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
#include "WeaponImpl.h"
#include "WeaponServicesImpl.h"
#include "WeaponVDataImpl.h"

#include <MemoryPatterns/MemoryPatterns.h>

#include <Platform/VmtFinder.h>

struct GameClassImplementations {
    GameClassImplementations(const MemoryPatterns& memoryPatterns, Tier0Dll tier0Dll) noexcept
        : conVar{memoryPatterns.conVarPatterns()}
        , cvar{memoryPatterns.cvarPatterns()}
        , entity{memoryPatterns.entityPatterns()}
        , entitySystem{memoryPatterns.entitySystemPatterns()}
        , fileNameSymbolTable{tier0Dll}
        , fileSystem{memoryPatterns.fileSystemPatterns()}
        , gameRules{memoryPatterns.gameRulesPatterns()}
        , gameSceneNode{memoryPatterns.gameSceneNodePatterns()}
        , hostageServicesImpl{memoryPatterns.hostageServicesPatterns()}
        , memAlloc{tier0Dll, memoryPatterns.memAllocPatterns()}
        , panel{memoryPatterns.panelPatterns()}
        , panelStyle{memoryPatterns.panelStylePatterns()}
        , imagePanel{memoryPatterns.panoramaImagePanelPatterns()}
        , panoramaLabel{memoryPatterns.panoramaLabelPatterns()}
        , uiEngine{memoryPatterns.panoramaUiEnginePatterns()}
        , panoramaUiPanelOffsets{memoryPatterns.panoramaUiPanelPatterns()}
        , plantedC4{memoryPatterns.plantedC4Patterns()}
        , playerController{memoryPatterns.playerControllerPatterns()}
        , playerPawn{memoryPatterns.playerPawnPatterns()}
        , topLevelWindow{memoryPatterns.topLevelWindowPatterns()}
        , weapon{memoryPatterns.weaponPatterns()}
        , weaponServices{memoryPatterns.weaponServicesPatterns()}
        , weaponVData{memoryPatterns.weaponVDataPatterns()}
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
    HostageServicesImpl hostageServicesImpl;
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
    WeaponImpl weapon;
    WeaponServicesImpl weaponServices;
    WeaponVDataImpl weaponVData;
};
