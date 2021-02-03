#pragma once

#include <array>
#include <deque>

#include "../ConfigStructs.h"

#include "../SDK/matrix3x4.h"
#include "../SDK/Vector.h"

enum class FrameStage;
struct UserCmd;

namespace Backtrack
{
    void update(FrameStage) noexcept;
    void run(UserCmd*) noexcept;

    struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[256];
    };

    const std::deque<Record>& getRecords(std::size_t index) noexcept;
    float getLerp() noexcept;
    bool valid(float simtime) noexcept;
    int timeToTicks(float time) noexcept;
    void init() noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
