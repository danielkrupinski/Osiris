#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/PanoramaUiEngine.h>

struct PanoramaSymbols {
    [[nodiscard]] static const PanoramaSymbols& instance() noexcept;

    cs2::CPanoramaSymbol deathNoticeKillerSymbol{PanoramaUiEngine::makeSymbol(0, "DeathNotice_Killer")};
    cs2::CPanoramaSymbol spawnTimeSymbol{PanoramaUiEngine::makeSymbol(0, "SpawnTime")};
};
