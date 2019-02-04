#pragma once

struct GlobalVars {
    const float realtime;
    const std::byte pad[12];
    const float currenttime;
    const float frametime;
    const int maxClients;
};
