#pragma once

#include "Pad.h"
#include "VirtualMethod.h"

namespace csgo
{

namespace pod
{

struct NetworkChannel {
    PAD(44)
    int chokedPackets;
};

}

class NetworkChannel : public VirtualCallableFromPOD<NetworkChannel, pod::NetworkChannel> {
public:
    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (flow))
};

}
