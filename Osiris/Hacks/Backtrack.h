#pragma once

#include <cstddef>
#include <algorithm>
#include <deque>

#include "../JsonForward.h"

#include "../SDK/matrix3x4.h"
#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Vector.h"
#include "../SDK/ModelRender.h"
#include "../SDK/NetworkChannel.h"

enum class FrameStage;
struct UserCmd;

#define OSIRIS_BACKTRACK() true

namespace Backtrack
{
    void update(FrameStage) noexcept;
    void run(UserCmd*) noexcept;
    void AddLatencyToNetwork(NetworkChannel*, float) noexcept;
    void UpdateIncomingSequences(bool reset = false) noexcept;
    struct BacktrackConfig {
        bool enabled = false;
        bool ignoreSmoke = false;
        bool recoilBasedFov = false;
        int timeLimit = 0;
        bool fakeLatency = false;
        bool drawAllChams = false;
    } backtrackConfig;

    struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[256];
    };

    const std::deque<Record>* getRecords(std::size_t index) noexcept;
    bool valid(float simtime) noexcept;
    void init() noexcept;
    struct IncomingSequence
    {
        int inreliablestate;
        int sequencenr;
        float servertime;
    };
    extern std::deque<IncomingSequence>sequences;

    float getExtraTicks() noexcept;
    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
