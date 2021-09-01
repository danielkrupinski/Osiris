#include <cmath>

#include "AntiAim.h"

#include "../imgui/imgui.h"

#include "../ConfigStructs.h"
#include "../SDK/Entity.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include <Hacks/Misc.h>
#include <imguiCustom.h>
#include <Memory.h>

float RandomFloat(float min, float max) noexcept
{
    return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}
#if OSIRIS_ANTIAIM()

struct AntiAimConfig {
    bool enabled = false;
    bool pitch = false;
    bool yaw = false;
    KeyBind aaKey;
} antiAimConfig;


void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (antiAimConfig.enabled) {
        if (!localPlayer)
            return;
        // is not on a ladder or use noclip (to skip that needs more proper movefix)
        if (localPlayer->moveType() == MoveType::LADDER || localPlayer->moveType() == MoveType::NOCLIP)
            return;
        const auto activeWeapon = localPlayer->getActiveWeapon();

        if (!activeWeapon)
            return;

        if (!activeWeapon->isGrenade() && cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))
            return;

        if (localPlayer->throwing(cmd))
            return;
        if (Misc::frozen)
            return;
        if (antiAimConfig.aaKey.isPressed())
        {
            if (Misc::flSide)
            {
                Misc::flSide = false;
            }
            else
            {
                Misc::flSide = true;
            }
            
        }
        if (antiAimConfig.pitch && !sendPacket && cmd->viewangles.x == currentViewAngles.x)
            cmd->viewangles.x = 90.f-RandomFloat(1.f, 3.f);
        if (antiAimConfig.yaw && !sendPacket && cmd->viewangles.y == currentViewAngles.y) {
            if (Misc::flSide)
            {
                cmd->viewangles.y += localPlayer->getMaxDesyncAngle()-RandomFloat(1.f,3.f); //right
            }
            else
            {
                cmd->viewangles.y -= localPlayer->getMaxDesyncAngle()-RandomFloat(1.f, 3.f); //left
            }
            
            cmd->buttons &= ~(UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVERIGHT | UserCmd::IN_MOVELEFT);
            if (std::abs(cmd->sidemove) < 5.0f) {
                if (cmd->buttons & UserCmd::IN_DUCK)
                    cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
                else
                    cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
            }
        }
    }
}

static bool antiAimOpen = false;

void AntiAim::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Anti aim")) {
        antiAimOpen = true;
        ImGui::SetWindowFocus("Anti aim");
        ImGui::SetWindowPos("Anti aim", { 100.0f, 100.0f });
    }
}

void AntiAim::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Anti aim")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

void AntiAim::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!antiAimOpen)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Anti aim", &antiAimOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }
    ImGui::Checkbox("Enabled", &antiAimConfig.enabled);
    ImGui::Checkbox("Pitch", &antiAimConfig.pitch);
    ImGui::Checkbox("Yaw", &antiAimConfig.yaw);
    ImGui::hotkey("", antiAimConfig.aaKey);
    if (!contentOnly)
        ImGui::End();
}

static void to_json(json& j, const AntiAimConfig& o, const AntiAimConfig& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Pitch", pitch);
    WRITE("Yaw", yaw);
    WRITE("aaKey", aaKey);
    
}

json AntiAim::toJson() noexcept
{
    json j;
    to_json(j, antiAimConfig);
    return j;
}

static void from_json(const json& j, AntiAimConfig& a)
{
    read(j, "Enabled", a.enabled);
    read(j, "Pitch", a.pitch);
    read(j, "Yaw", a.yaw);
    read(j, "aaKey", a.aaKey);
}

void AntiAim::fromJson(const json& j) noexcept
{
    from_json(j, antiAimConfig);
}

void AntiAim::resetConfig() noexcept
{
    antiAimConfig = { };
}

#else

namespace AntiAim
{
    void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept {}

    // GUI
    void menuBarItem() noexcept {}
    void tabItem() noexcept {}
    void drawGUI(bool contentOnly) noexcept {}

    // Config
    json toJson() noexcept { return {}; }
    void fromJson(const json& j) noexcept {}
    void resetConfig() noexcept {}
}

#endif
