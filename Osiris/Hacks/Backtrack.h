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

    extern Cvars cvars; // This to be removed (?) to comply with master

    float getLerp() noexcept;
    bool valid(float simtime) noexcept;

    struct IncomingSequence
    {
        int inreliablestate;
        int sequencenr;
        float servertime;
    };

    extern std::deque<IncomingSequence>sequences;
    extern 
    float getExtraTicks() noexcept;

    int timeToTicks(float time) noexcept;
    void init() noexcept;
}
