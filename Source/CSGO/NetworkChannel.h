#pragma once

#include <Utils/Pad.h>
#include "VirtualMethod.h"

namespace csgo
{

struct NetworkChannelPOD {
    PAD(44);
    int chokedPackets;
};

struct NetworkChannel : GameClass<NetworkChannel, NetworkChannelPOD> {
    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (flow))
};

}
