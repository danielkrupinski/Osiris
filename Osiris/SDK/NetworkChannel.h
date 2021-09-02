#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class NetworkChannel {
public:
    INCONSTRUCTIBLE(NetworkChannel)

    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (this, flow))

    std::byte pad[24];
    int OutSequenceNr;
    int InSequenceNr;
    int OutSequenceNrAck;
    int OutReliableState;
    int InReliableState;
    int chokedPackets;
};
class NetworkMessage
{
public:
    VIRTUAL_METHOD(int, getType, 7, (), (this))
};