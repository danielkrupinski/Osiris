#pragma once

#include <deque>
#include <algorithm>
#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Cvar.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/ModelRender.h"

enum class FrameStage;
struct UserCmd;

namespace Backtrack {
    void update(FrameStage) noexcept;
    void run(UserCmd*) noexcept;

    struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[128];
    };

    extern std::deque<Record> records[65];

    struct Cvars {
        Cvar* updateRate;
        Cvar* maxUpdateRate;
        Cvar* interp;
        Cvar* interpRatio;
        Cvar* minInterpRatio;
        Cvar* maxInterpRatio;
        Cvar* maxUnlag;
    };

    extern Cvars cvars;

    constexpr auto getLerp() noexcept
    {
        auto ratio = std::clamp(cvars.interpRatio->getFloat(), cvars.minInterpRatio->getFloat(), cvars.maxInterpRatio->getFloat());

        return max(cvars.interp->getFloat(), (ratio / ((cvars.maxUpdateRate) ? cvars.maxUpdateRate->getFloat() : cvars.updateRate->getFloat())));
    }

    constexpr auto valid(float simtime) noexcept
    {
        auto network = interfaces.engine->getNetworkChannel();
        if (!network)
            return false;

        auto delta = std::clamp(network->getLatency(0) + getLerp(), 0.f, cvars.maxUnlag->getFloat()) - (memory.globalVars->currenttime - simtime);
        return std::fabsf(delta) <= 0.2f;
    }

    constexpr auto timeToTicks(float time) noexcept
    {
        return static_cast<int>(0.5f + time / memory.globalVars->intervalPerTick);
    }

    static void init() noexcept
    {
        records->clear();

        cvars.updateRate = interfaces.cvar->findVar("cl_updaterate");
        cvars.maxUpdateRate = interfaces.cvar->findVar("sv_maxupdaterate");
        cvars.interp = interfaces.cvar->findVar("cl_interp");
        cvars.interpRatio = interfaces.cvar->findVar("cl_interp_ratio");
        cvars.minInterpRatio = interfaces.cvar->findVar("sv_client_min_interp_ratio");
        cvars.maxInterpRatio = interfaces.cvar->findVar("sv_client_max_interp_ratio");
        cvars.maxUnlag = interfaces.cvar->findVar("sv_maxunlag");
    }
}
