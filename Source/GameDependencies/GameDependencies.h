#pragma once

#include "ConVars.h"
#include "EntitiesVMTs.h"
#include "FileNameSymbolTableDeps.h"
#include "GlowSceneObjectDeps.h"
#include "HudDeps.h"
#include "MemAllocDeps.h"
#include "PanoramaSymbols.h"

#include <FeatureHelpers/StylePropertiesSymbolsAndVMTs.h>

#include <MemoryPatterns/MemoryPatterns.h>
#include <Platform/VmtFinder.h>

struct GameDependencies {
    GameDependencies() = default;

    GameDependencies(const MemoryPatterns& memoryPatterns, const VmtFinder& clientVmtFinder, const VmtFinder& panoramaVmtFinder, Tier0Dll tier0Dll) noexcept
        : fileNameSymbolTableDeps{tier0Dll}
        , memAllocDeps{tier0Dll}
        , stylePropertiesSymbolsAndVMTs{StylePropertySymbolMap{memoryPatterns.panelStylePatterns().stylePropertiesSymbols()}, panoramaVmtFinder}
        , entitiesVMTs{clientVmtFinder}
    {
    }

    FileNameSymbolTableDeps fileNameSymbolTableDeps;
    GlowSceneObjectDeps glowSceneObjectDeps;
    HudDeps hudDeps;
    MemAllocDeps memAllocDeps;

    StylePropertiesSymbolsAndVMTs stylePropertiesSymbolsAndVMTs;
    EntitiesVMTs entitiesVMTs;
    std::optional<ConVars> conVars;
    std::optional<PanoramaSymbols> panoramaSymbols;
};
