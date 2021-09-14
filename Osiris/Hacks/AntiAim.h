#pragma once

#include "../JsonForward.h"

struct UserCmd;
struct Vector;
enum class FrameStage;
class GameEvent;
struct ImDrawList;

#define OSIRIS_ANTIAIM() true

namespace AntiAim
{
    bool frozen = false;
    bool indicatorsa = false;
    void run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept;
    void frozenaa(GameEvent* event) noexcept;
    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    // Config
    json toJson() noexcept;
    static bool invertw = true;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}


