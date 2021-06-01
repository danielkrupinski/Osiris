#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class NetworkChannel {
public:
    INCONSTRUCTIBLE(NetworkChannel)

    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (this, flow))

    std::byte pad[44];
    int chokedPackets;
};
