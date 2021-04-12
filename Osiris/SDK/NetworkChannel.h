#pragma once

#include "VirtualMethod.h"

class NetworkChannel {
public:
    VIRTUAL_METHOD(float, getLatency, IS_WIN32() ? 9 : 8, (int flow), (this, flow))

    std::byte pad[WIN32_LINUX(43, 63)];

    int chokedPackets;
};
