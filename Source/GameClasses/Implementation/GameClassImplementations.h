#pragma once

#include "ClientModeImpl.h"
#include "FileNameSymbolTableImpl.h"
#include "FileSystemImpl.h"
#include "GameRulesImpl.h"
#include "MemAllocImpl.h"
#include "PanoramaImagePanelImpl.h"
#include "PanoramaLabelImpl.h"
#include "PanoramaUiEngineImpl.h"
#include "PanoramaUiPanelImpl.h"
#include "PlantedC4Impl.h"

struct GameClassImplementations {
    explicit GameClassImplementations(Tier0Dll tier0Dll) noexcept
        : fileNameSymbolTable{ tier0Dll }
        , memAlloc{ tier0Dll }
    {
    }

    ClientModeImpl clientMode;
    FileNameSymbolTableImpl fileNameSymbolTable;
    FileSystemImpl fileSystem;
    GameRulesImpl gameRules;
    MemAllocImpl memAlloc;
    PanoramaImagePanelImpl imagePanel;
    PanoramaLabelImpl panoramaLabel;
    PanoramaUiEngineImpl uiEngine;
    PanoramaUiPanelImpl panoramaUiPanelOffsets;
    PlantedC4Impl plantedC4;
};
