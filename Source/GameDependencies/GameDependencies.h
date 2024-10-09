#pragma once

#include "ConVars.h"
#include "ConVarDeps.h"
#include "CvarDeps.h"
#include "EntitiesVMTs.h"
#include "FileNameSymbolTableDeps.h"
#include "FileSystemDeps.h"
#include "GlowSceneObjectDeps.h"
#include "HudDeps.h"
#include "MemAllocDeps.h"
#include "PanelStyleDeps.h"
#include "PanoramaSymbols.h"
#include "PanoramaUiEngineDeps.h"
#include "PanoramaUiPanelDeps.h"
#include "PlantedC4Deps.h"
#include "PlayerControllerDeps.h"
#include "PlayerPawnDeps.h"
#include "RenderComponentDeps.h"
#include "SceneObjectDeps.h"
#include "SceneObjectUpdaterDeps.h"
#include "SceneSystemDeps.h"
#include "SmokeGrenadeProjectileDeps.h"
#include "TopLevelWindowDeps.h"
#include "WeaponDeps.h"
#include "WeaponServicesDeps.h"
#include "WeaponVDataDeps.h"

#include <FeatureHelpers/StylePropertiesSymbolsAndVMTs.h>

#include <MemoryPatterns/MemoryPatterns.h>
#include <Platform/VmtFinder.h>

struct GameDependencies {
    GameDependencies() = default;

    GameDependencies(const MemoryPatterns& memoryPatterns, const VmtFinder& clientVmtFinder, const VmtFinder& panoramaVmtFinder, Tier0Dll tier0Dll) noexcept
        : conVarDeps{memoryPatterns.conVarPatterns()}
        , cvarDeps{memoryPatterns.cvarPatterns()}
        , fileNameSymbolTableDeps{tier0Dll}
        , fileSystemDeps{memoryPatterns.fileSystemPatterns()}
        , memAllocDeps{tier0Dll}
        , panelStyleDeps{memoryPatterns.panelStylePatterns()}
        , uiEngineDeps{memoryPatterns.panoramaUiEnginePatterns()}
        , panoramaUiPanelDeps{memoryPatterns.panoramaUiPanelPatterns()}
        , plantedC4Deps{memoryPatterns.plantedC4Patterns()}
        , playerControllerDeps{memoryPatterns.playerControllerPatterns()}
        , playerPawnDeps{memoryPatterns.playerPawnPatterns()}
        , renderComponentDeps{memoryPatterns.renderComponentPatterns()}
        , sceneObjectDeps{memoryPatterns.sceneObjectPatterns()}
        , sceneObjectUpdaterDeps{memoryPatterns.sceneObjectUpdaterPatterns()}
        , sceneSystemDeps{memoryPatterns.sceneSystemPatterns()}
        , smokeGrenadeProjectileDeps{memoryPatterns.smokeGrenadeProjectilePatterns()}
        , topLevelWindowDeps{memoryPatterns.topLevelWindowPatterns()}
        , weaponDeps{memoryPatterns.weaponPatterns()}
        , weaponServicesDeps{memoryPatterns.weaponServicesPatterns()}
        , weaponVDataDeps{memoryPatterns.weaponVDataPatterns()}
        , stylePropertiesSymbolsAndVMTs{StylePropertySymbolMap{memoryPatterns.panelStylePatterns().stylePropertiesSymbols()}, panoramaVmtFinder}
        , entitiesVMTs{clientVmtFinder}
        , soundChannels{memoryPatterns.soundSystemPatterns().soundChannels()}
        , fileSystem{memoryPatterns.fileSystemPatterns().fileSystem()}
    {
    }

    ConVarDeps conVarDeps;
    CvarDeps cvarDeps;
    FileNameSymbolTableDeps fileNameSymbolTableDeps;
    FileSystemDeps fileSystemDeps;
    GlowSceneObjectDeps glowSceneObjectDeps;
    HudDeps hudDeps;
    MemAllocDeps memAllocDeps;
    PanelStyleDeps panelStyleDeps;
    PanoramaUiEngineDeps uiEngineDeps;
    PanoramaUiPanelDeps panoramaUiPanelDeps;
    PlantedC4Deps plantedC4Deps;
    PlayerControllerDeps playerControllerDeps;
    PlayerPawnDeps playerPawnDeps;
    RenderComponentDeps renderComponentDeps;
    SceneObjectDeps sceneObjectDeps;
    SceneObjectUpdaterDeps sceneObjectUpdaterDeps;
    SceneSystemDeps sceneSystemDeps;
    SmokeGrenadeProjectileDeps smokeGrenadeProjectileDeps;
    TopLevelWindowDeps topLevelWindowDeps;
    WeaponDeps weaponDeps;
    WeaponServicesDeps weaponServicesDeps;
    WeaponVDataDeps weaponVDataDeps;

    StylePropertiesSymbolsAndVMTs stylePropertiesSymbolsAndVMTs;
    EntitiesVMTs entitiesVMTs;
    cs2::SoundChannels** soundChannels;
    cs2::CBaseFileSystem** fileSystem;
    std::optional<ConVars> conVars;
    std::optional<PanoramaSymbols> panoramaSymbols;
};
