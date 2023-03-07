#pragma once

#include <array>
#include <cstddef>
#include <deque>

#include "../JsonForward.h"

#include <CSGO/matrix3x4.h>
#include <CSGO/Vector.h>
#include <CSGO/ConVar.h>

#include "../Memory.h"

namespace csgo
{
    enum class FrameStage;
    struct UserCmd;
    struct Cvar;
}

class ClientInterfaces;
class EngineInterfaces;

class Backtrack {
public:
    explicit Backtrack(const csgo::Cvar& cvar);

    void update(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, csgo::FrameStage) noexcept;
    void run(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, csgo::UserCmd*) noexcept;

    struct Record {
        csgo::Vector origin;
        float simulationTime;
        csgo::matrix3x4 matrix[256];
    };

    std::deque<Record>* getRecords(std::size_t index) noexcept;
    bool valid(const csgo::Engine& engine, const Memory& memory, float simtime) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;
  
    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Enabled", enabled).def(false);
        configurator("Ignore smoke", ignoreSmoke).def(false);
        configurator("Recoil based fov", recoilBasedFov).def(false);
        configurator("Time limit", timeLimit).def(200);
    }

private:
    float getLerp() noexcept;

    struct Cvars {
        csgo::ConVar updateRate;
        csgo::ConVar maxUpdateRate;
        csgo::ConVar interp;
        csgo::ConVar interpRatio;
        csgo::ConVar minInterpRatio;
        csgo::ConVar maxInterpRatio;
        csgo::ConVar maxUnlag;
    };

    bool enabled;
    bool ignoreSmoke;
    bool recoilBasedFov;
    int timeLimit;
    Cvars cvars;
    std::array<std::deque<Record>, 65> records;
    bool backtrackWindowOpen = false;
};
