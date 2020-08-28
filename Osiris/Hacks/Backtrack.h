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
	void AddLatencyToNetwork(NetworkChannel*, float) noexcept;
    void UpdateIncomingSequences(bool reset = false) noexcept;

    struct Record {
        Vector head;
        Vector origin;
        float resolvedaddition = 0.0f;
        float simulationTime = 0.0f;
        matrix3x4 matrix[256];
        int PreviousAct = 0;
        int prevhealth = 0;
        matrix3x4 prevResolvedMatrix[256];
        int lastworkmissed = 0;
        int missedshots = 0;
        bool wasTargeted = 0;
        bool invalid = false;
        bool wasUpdated = false;
        Vector mins;
        Vector max; 
        float alpha = 100; //ONLY USED FOR extended_records
    };
	
    extern std::deque<Record> records[65];
	extern std::deque<Record> extended_records[65];
	extern std::deque<Record> invalid_record[2];

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

	struct IncomingSequence
    {
        int inreliablestate;
        int sequencenr;
        float servertime;
    };

   extern std::deque<IncomingSequence>sequences;

    constexpr auto getLerp() noexcept
    {
        auto ratio = std::clamp(cvars.interpRatio->getFloat(), cvars.minInterpRatio->getFloat(), cvars.maxInterpRatio->getFloat());

        return max(cvars.interp->getFloat(), (ratio / ((cvars.maxUpdateRate) ? cvars.maxUpdateRate->getFloat() : cvars.updateRate->getFloat())));
    }

    constexpr auto valid(float simtime) noexcept
    {
        auto network = interfaces->engine->getNetworkChannel();
        if (!network)
            return false;

        auto delta = std::clamp(network->getLatency(0) + network->getLatency(1) + getLerp(), 0.f, cvars.maxUnlag->getFloat()) - (memory->globalVars->serverTime() - simtime);
        return std::fabsf(delta) <= 0.2f;
    }

    constexpr float getExtraTicks() noexcept
    {
        auto network = interfaces->engine->getNetworkChannel();
        if (!network)
            return 0.f;

        return std::clamp(network->getLatency(1) - network->getLatency(0), 0.f, cvars.maxUnlag->getFloat());
    }

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
