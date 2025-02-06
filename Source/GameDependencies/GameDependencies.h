#pragma once

#include <optional>

#include "ConVars.h"
#include "FileNameSymbolTableDeps.h"
#include "GlowSceneObjectDeps.h"
#include "HudDeps.h"
#include "MemAllocDeps.h"

#include <GameDLLs/Tier0Dll.h>
#include <MemoryPatterns/MemoryPatterns.h>
#include <Panorama/PanoramaSymbols.h>
#include <Panorama/StylePropertiesSymbolsAndVMTs.h>
#include <Panorama/StylePropertySymbolMap.h>
#include <Platform/VmtFinder.h>

struct GameDependencies {
    GameDependencies() = default;

    GameDependencies(const MemoryPatterns& memoryPatterns, const VmtFinder& panoramaVmtFinder, Tier0Dll tier0Dll) noexcept
        : fileNameSymbolTableDeps{tier0Dll}
        , memAllocDeps{tier0Dll}
        , stylePropertiesSymbolsAndVMTs{StylePropertySymbolMap{memoryPatterns.panelStylePatterns().stylePropertiesSymbols()}, panoramaVmtFinder}
    {
    }

    FileNameSymbolTableDeps fileNameSymbolTableDeps;
    GlowSceneObjectDeps glowSceneObjectDeps;
    HudDeps hudDeps;
    MemAllocDeps memAllocDeps;

    StylePropertiesSymbolsAndVMTs stylePropertiesSymbolsAndVMTs;
    std::optional<ConVars> conVars;
    std::optional<PanoramaSymbols> panoramaSymbols;
};
