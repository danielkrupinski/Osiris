#pragma once

#include "VirtualMethod.h"

class bf_write;

class NetworkChannel {
public:
    VIRTUAL_METHOD(float, getLatency, 9, (int flow), (this, flow))

    std::byte pad[24];
    int OutSequenceNr;
    int InSequenceNr;
    int OutSequenceNrAck;
    int OutReliableState;
    int InReliableState;
    int chokedPackets;
};
