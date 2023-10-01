#pragma once

#include <CS2/Classes/Panorama.h>
#include "Implementation/PanoramaLabelImpl.h"

struct PanoramaLabel {
    explicit PanoramaLabel(cs2::CLabel* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    void setTextInternal(const char* value, int textType, bool trustedSource) const noexcept
    {
        if (PanoramaLabelImpl::instance().setTextInternal)
            PanoramaLabelImpl::instance().setTextInternal(thisptr, value, textType, trustedSource);
    }

private:
    cs2::CLabel* thisptr;
};
