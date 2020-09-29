#pragma once
#include <algorithm>
#include <array>
#include <comcat.h>
#include <deque>
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/ConVar.h"
#include "../SDK/matrix3x4.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/Vector.h"

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

	float getExtraTicks() noexcept;
	const std::deque<Record>& getRecords(std::size_t index) noexcept;
    float getLerp() noexcept;
    bool valid(float simtime) noexcept;
    int timeToTicks(float time) noexcept;
    void init() noexcept;
}
