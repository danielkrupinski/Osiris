#pragma once

#include "Pad.h"
#include "VirtualMethod.h"

namespace csgo
{

struct NetworkChannelPOD {
    PAD(44);
    int chokedPackets;
};

struct NetworkChannel : VirtualCallableFromPOD<NetworkChannel, NetworkChannelPOD> {
    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (flow))
};

}
