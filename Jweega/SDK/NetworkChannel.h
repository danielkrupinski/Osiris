#pragma once

#include "Utils.h"

class NetworkChannel {
public:
    constexpr auto getLatency(int flow) noexcept
    {
        return callVirtualMethod<float, int>(this, 9, flow);
    }

    constexpr auto getAvgLatency(int flow) noexcept
    {
        return callVirtualMethod<float, int>(this, 10, flow);
    }

    std::byte pad[44];
    int chokedPackets;
};
