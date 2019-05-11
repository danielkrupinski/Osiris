#pragma once

#include <deque>
#include <algorithm>
#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/UserCmd.h"
#include "../SDK/FrameStage.h"

namespace Backtrack {
	void init() noexcept;
	void update(FrameStage) noexcept;
	void run(UserCmd*) noexcept;

	struct Record {
		Vector absOrigin;
		Vector mins;
		Vector maxs;
		Vector head;
		int flags;
		float simulationTime;
        int layerOrder[15];
        int layerSequence[15];
        float layerWeight[15];
        float layerCycle[15];
        matrix3x4 matrix[128];
    };

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

    constexpr float getLerp() noexcept
    {
        auto ratio = cvars.interpRatio->getFloat();
        ratio = std::clamp(ratio, (cvars.minInterpRatio->getFloat() != 1.f) ? cvars.minInterpRatio->getFloat() : 1.f, cvars.maxInterpRatio->getFloat());

        return max(cvars.interp->getFloat(), (ratio / ((cvars.maxUpdateRate) ? cvars.maxUpdateRate->getFloat() : cvars.updateRate->getFloat())));
    }

    constexpr bool valid(float simtime) noexcept
    {
        auto lerp = getLerp();
        auto network = interfaces.engine->getNetworkChannel();
        if (!network)
            return false;

        auto delta = std::clamp(network->getLatency(0) + lerp, 0.f, cvars.maxUnlag->getFloat()) - (memory.globalVars->currenttime - simtime);
        return (std::fabsf(delta) < 0.2f);
    }

    constexpr int timeToTicks(float time) noexcept
    {
        return (int)(0.5f + (float)(time) / memory.globalVars->intervalPerTick);
    }

    extern std::deque<Record> records[65];
}