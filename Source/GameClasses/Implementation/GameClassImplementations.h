#pragma once

#include "EntitySystemImpl.h"
#include "FileNameSymbolTableImpl.h"
#include "FileSystemImpl.h"
#include "GameRulesImpl.h"
#include "MemAllocImpl.h"
#include "PanelImpl.h"
#include "PanelStyleImpl.h"
#include "PanoramaImagePanelImpl.h"
#include "PanoramaLabelImpl.h"
#include "PanoramaUiEngineImpl.h"
#include "PanoramaUiPanelImpl.h"
#include "PlantedC4Impl.h"
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
                             const TopLevelWindowPatterns& topLevelWindowPatterns,
                             Tier0Dll tier0Dll) noexcept
        : entitySystem{entitySystemPatterns}
        , fileNameSymbolTable{tier0Dll}
        , fileSystem{fileSystemPatterns}
        , gameRules{gameRulesPatterns}
        , memAlloc{tier0Dll, memAllocPatterns}
        , panel{panelPatterns}
        , panelStyle{panelStylePatterns}
        , imagePanel{panoramaImagePanelPatterns}
        , panoramaLabel{panoramaLabelPatterns}
        , uiEngine{clientPatterns, panoramaUiEnginePatterns}
        , panoramaUiPanelOffsets{panoramaUiPanelPatterns}
        , plantedC4{plantedC4Patterns}
        , topLevelWindow{topLevelWindowPatterns}
    {
    }

    EntitySystemImpl entitySystem;
    FileNameSymbolTableImpl fileNameSymbolTable;
    FileSystemImpl fileSystem;
    GameRulesImpl gameRules;
    MemAllocImpl memAlloc;
    PanelImpl panel;
    PanelStyleImpl panelStyle;
    PanoramaImagePanelImpl imagePanel;
    PanoramaLabelImpl panoramaLabel;
    PanoramaUiEngineImpl uiEngine;
    PanoramaUiPanelImpl panoramaUiPanelOffsets;
    PlantedC4Impl plantedC4;
    TopLevelWindowImpl topLevelWindow;
};
