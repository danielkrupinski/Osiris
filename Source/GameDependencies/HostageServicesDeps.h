#pragma once

#include <GameClasses/OffsetTypes/HostageServicesOffset.h>

struct HostageServicesDeps {
    template <typename HostageServicesPatterns>
    explicit HostageServicesDeps(const HostageServicesPatterns& hostageServicesPatterns) noexcept
        : offsetToCarriedHostage{hostageServicesPatterns.offsetToCarriedHostage()}
    {
    }

    OffsetToCarriedHostage offsetToCarriedHostage;
};
