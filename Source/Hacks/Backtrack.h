#pragma once

#include <array>
#include <cstddef>
#include <deque>

#include "../JsonForward.h"

#include <SDK/matrix3x4.h>
#include <SDK/Vector.h>
#include <SDK/ConVar.h>

#include "../Memory.h"

namespace csgo { enum class FrameStage; }
struct UserCmd;
class ClientInterfaces;
class Cvar;
class EngineInterfaces;

class Backtrack {
public:
    explicit Backtrack(const Cvar& cvar);

    void update(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, csgo::FrameStage) noexcept;
    void run(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, UserCmd*) noexcept;

    struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[256];
    };

    const std::deque<Record>* getRecords(std::size_t index) noexcept;
    bool valid(const Engine& engine, const Memory& memory, float simtime) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;

private:
    float getLerp() noexcept;

    struct Cvars {
        ConVar updateRate;
        ConVar maxUpdateRate;
        ConVar interp;
        ConVar interpRatio;
        ConVar minInterpRatio;
        ConVar maxInterpRatio;
        ConVar maxUnlag;
    };

    bool enabled = false;
    bool ignoreSmoke = false;
    bool recoilBasedFov = false;
    int timeLimit = 200;
    Cvars cvars;
    std::array<std::deque<Record>, 65> records;
    bool backtrackWindowOpen = false;
};
