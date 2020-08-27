#pragma once

#include "VirtualMethod.h"

class NetworkChannel {
public:
    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (this, flow))

    std::byte pad[44];
    int chokedPackets;
};
