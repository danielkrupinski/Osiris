#pragma once

#include <Utils/Pad.h>

namespace csgo
{

struct ClientState {};

struct SplitScreen {
    void* vmt;

    struct SplitPlayer {
        PAD(8);
        ClientState client;
    };

    SplitPlayer* splitScreenPlayers[1];
};

}
