#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameDependencies/PanoramaLabelDeps.h>

struct PanoramaLabel {
    explicit PanoramaLabel(cs2::CLabel* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    void setTextInternal(const char* value, int textType, bool trustedSource) const noexcept
    {
        if (PanoramaLabelDeps::instance().setTextInternal)
            PanoramaLabelDeps::instance().setTextInternal(thisptr, value, textType, trustedSource);
    }

private:
    cs2::CLabel* thisptr;
};
