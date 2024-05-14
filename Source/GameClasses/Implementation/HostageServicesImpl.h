#pragma once

#include <GameClasses/OffsetTypes/HostageServicesOffset.h>

struct HostageServicesImpl {
    template <typename HostageServicesPatterns>
    explicit HostageServicesImpl(const HostageServicesPatterns& hostageServicesPatterns) noexcept
        : offsetToCarriedHostage{hostageServicesPatterns.offsetToCarriedHostage()}
    {
    }

    OffsetToCarriedHostage offsetToCarriedHostage;
};
