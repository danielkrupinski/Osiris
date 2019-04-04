#pragma once

#include "../Interfaces.h"

struct GlobalVars {
    const float realtime;
    const std::byte pad[12];
    const float currenttime;
    const float frametime;
    const int maxClients;
    const int tickCount;
    const float intervalPerTick;

    constexpr float serverTime() noexcept
    {
        return interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_nTickBase") * intervalPerTick;
    }
};
