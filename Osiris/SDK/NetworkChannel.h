#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class NetworkChannel {
public:
    INCONSTRUCTIBLE(NetworkChannel)

    VIRTUAL_METHOD(float, getLatency, IS_WIN32() ? 9 : 8, (int flow), (this, flow))

    std::byte pad[WIN32_LINUX(43, 63)];
    int chokedPackets;
};
