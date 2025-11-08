#pragma once

#include <Config/ConfigState.h>
#include <CS2/Classes/CLoopModeGame.h>
#include <GameClient/ConVars/ConVarsBase.h>
#include <GameClient/FileNameSymbolTableState.h>
#include <GameClient/Hud/HudState.h>
#include <GameClient/MemAllocState.h>
#include <GameClient/Panorama/PanoramaSymbols.h>
#include <OutlineGlow/GlowSceneObjectState.h>
#include <GameClient/DLLs/Tier0Dll.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Common/RenderingHookEntityLoop.h>
#include <Features/Sound/Details/SoundWatcher.h>
#include <Features/Common/InWorldPanelsState.h>
#include <Features/FeaturesStates.h>
#include <Features/Visuals/ModelGlow/Preview/PlayerModelGlowPreviewState.h>
#include <Features/Visuals/ModelGlow/Preview/WeaponModelGlowPreviewState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheState.h>
#include <MemoryPatterns/AllMemoryPatternSearchResults.h>
#include <MemoryPatterns/MemoryPatterns.h>
#include <MemorySearch/PatternNotFoundLogger.h>
#include "UnloadFlag.h"
#include <Hooks/Hooks.h>
#include <Hooks/PeepEventsHook.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelManager.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelManagerContext.h>
#include <GameClient/Hud/BombStatus/BombStatusPanelState.h>
#include <MemorySearch/PatternFinder.h>
#include <MemorySearch/PatternSearchResults.h>
#include <UI/Panorama/PanoramaGUI.h>
#include <UI/Panorama/PanoramaGuiState.h>
#include <OutlineGlow/GlowSceneObjectsState.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/VmtFinder.h>
#include <Vmt/VmtLengthCalculator.h>

#include "OsirisDirectoryPath.h"

#include <CS2/Classes/ConVarTypes.h>

struct FullGlobalContext {
    FullGlobalContext(PeepEventsHook peepEventsHook, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, const MemoryPatterns& memoryPatterns, Tier0Dll tier0Dll) noexcept
        : patternSearchResults{memoryPatterns}
        , fileNameSymbolTableState{tier0Dll}
        , memAllocState{tier0Dll}
        , stylePropertySymbolsAndVMTs{StylePropertySymbolMap{memoryPatterns.panelStylePatterns().stylePropertiesSymbols()}, VmtFinder{panoramaDLL.getVmtFinderParams()}}
        , hooks{
            peepEventsHook,
            patternSearchResults.get<ViewRenderPointer>(),
            VmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()}}
    {
    }

    OsirisDirectoryPath osirisDirectoryPath;
    ConfigState configState;
    AllMemoryPatternSearchResults patternSearchResults;
    FileNameSymbolTableState fileNameSymbolTableState;
    GlowSceneObjectState glowSceneObjectState;
    HudState hudState;
    MemAllocState memAllocState;
    StylePropertiesSymbolsAndVMTs stylePropertySymbolsAndVMTs;
    std::optional<ConVarsBase> conVars;
    std::optional<PanoramaSymbols> panoramaSymbols;
    Hooks hooks;
    SoundWatcherState soundWatcherState;
    FeaturesStates featuresStates;
    PanoramaGuiState panoramaGuiState;
    BombStatusPanelState bombStatusPanelState;
    InWorldPanelsState inWorldPanelsState;
    GlowSceneObjectsState glowSceneObjectsState;
    EntityClassifier entityClassifier;
    PlayerInfoPanelCacheState playerInfoPanelCacheState;
    PlayerModelGlowPreviewState playerModelGlowPreviewState;
    WeaponModelGlowPreviewState weaponModelGlowPreviewState;
};
