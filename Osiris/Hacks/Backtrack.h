#pragma once

#include <algorithm>
#include <deque>

#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/matrix3x4.h"
#include "../SDK/ModelRender.h"
#include "../SDK/NetworkChannel.h"

enum class FrameStage;
struct UserCmd;

namespace Backtrack {
    void update(FrameStage) noexcept;
    void run(UserCmd*) noexcept;

    struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[256];
    };

    extern std::deque<Record> records[65];

    struct Cvars {
        ConVar* updateRate;
        ConVar* maxUpdateRate;
        ConVar* interp;
        ConVar* interpRatio;
        ConVar* minInterpRatio;
        ConVar* maxInterpRatio;
        ConVar* maxUnlag;
    };

    extern Cvars cvars;

    float getLerp() noexcept;
    bool valid(float simtime) noexcept;
    int timeToTicks(float time) noexcept;

    static void init() noexcept
    {
        records->clear();

        cvars.updateRate = interfaces->cvar->findVar("cl_updaterate");
        cvars.maxUpdateRate = interfaces->cvar->findVar("sv_maxupdaterate");
        cvars.interp = interfaces->cvar->findVar("cl_interp");
        cvars.interpRatio = interfaces->cvar->findVar("cl_interp_ratio");
        cvars.minInterpRatio = interfaces->cvar->findVar("sv_client_min_interp_ratio");
        cvars.maxInterpRatio = interfaces->cvar->findVar("sv_client_max_interp_ratio");
        cvars.maxUnlag = interfaces->cvar->findVar("sv_maxunlag");
    }
}
