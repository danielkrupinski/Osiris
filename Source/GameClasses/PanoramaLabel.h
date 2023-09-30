#pragma once

#include <CS2/Classes/Panorama.h>
#include "Implementation/PanoramaLabelImpl.h"
#include <Verification/VerifyMacros.h>

struct PanoramaLabel {
    explicit PanoramaLabel(cs2::CLabel* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    void setTextInternal(const char* value, int textType, bool trustedSource) const noexcept
    {
        //VERIFY_NE("Function pointer must not be null", PanoramaLabelImpl::instance().setTextInternal, nullptr);
        if (PanoramaLabelImpl::instance().setTextInternal)
            PanoramaLabelImpl::instance().setTextInternal(thisptr, value, textType, trustedSource);
    }

private:
    cs2::CLabel* thisptr;
};
