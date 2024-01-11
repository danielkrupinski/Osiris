#pragma once

#include <CS2/Classes/Panorama.h>
#include <Helpers/PanoramaPanelPointer.h>

struct KillfeedPreserverState {
    bool enabled{false};

    cs2::CPanoramaSymbol deathNoticeKillerSymbol{-1};
    cs2::CPanoramaSymbol spawnTimeSymbol{-1};
    PanoramaPanelPointer deathNoticesPointer;
};
