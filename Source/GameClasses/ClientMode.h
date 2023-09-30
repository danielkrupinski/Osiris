#pragma once

#include <cstddef>

#include "Implementation/ClientModeImpl.h"

struct ClientMode {
    explicit ClientMode(cs2::ClientModeCSNormal* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

    void removeZoomedSniperEffect() const noexcept
    {
        ClientModeImpl::instance().zoomedSniperEffectWeight.of(thisptr) = 0.0f;
        ClientModeImpl::instance().zoomedMovingSniperEffectWeight.of(thisptr) = 0.0f;
    }

private:
    cs2::ClientModeCSNormal* thisptr;
};
