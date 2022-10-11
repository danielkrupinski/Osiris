#pragma once

#include <cstddef>
#include <deque>

#include "../JsonForward.h"

#include "../SDK/matrix3x4.h"
#include "../SDK/Vector.h"

#include "../Memory.h"

namespace csgo { enum class FrameStage; }
struct UserCmd;

#define OSIRIS_BACKTRACK() true

namespace Backtrack
{
    void update(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const Interfaces& interfaces, const Memory& memory, csgo::FrameStage) noexcept;
    void run(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const Interfaces& interfaces, const Memory& memory, UserCmd*) noexcept;

    struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[256];
    };

    const std::deque<Record>* getRecords(std::size_t index) noexcept;
    bool valid(const Engine& engine, const Memory& memory, float simtime) noexcept;
    void init(const Interfaces& interfaces) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
