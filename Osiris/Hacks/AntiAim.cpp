#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"

void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (config->antiAim.enabled) {
        if (!localPlayer)
            return;

        if (config->antiAim.pitch && cmd->viewangles.x == currentViewAngles.x)
            cmd->viewangles.x = config->antiAim.pitchAngle;

        if (config->antiAim.yaw && !sendPacket && cmd->viewangles.y == currentViewAngles.y) {
            cmd->viewangles.y += localPlayer->getMaxDesyncAngle();
            if (fabsf(cmd->sidemove) < 5.0f) {
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
    if (ImGui::MenuItem("Glow")) {
        antiAimOpen = true;
        ImGui::SetWindowFocus("Glow");
        ImGui::SetWindowPos("Glow", { 100.0f, 100.0f });
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
    ImGui::Checkbox("Enabled", &config->antiAim.enabled);
    ImGui::Checkbox("##pitch", &config->antiAim.pitch);
    ImGui::SameLine();
    ImGui::SliderFloat("Pitch", &config->antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
    ImGui::Checkbox("Yaw", &config->antiAim.yaw);
    if (!contentOnly)
        ImGui::End();
}

static void to_json(json& j, const Config::AntiAim& o, const Config::AntiAim& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Pitch", pitch);
    WRITE("Pitch angle", pitchAngle);
    WRITE("Yaw", yaw);
}

json AntiAim::toJson() noexcept
{
    json j;
    to_json(j, config->antiAim);
    return j;
}

static void from_json(const json& j, Config::AntiAim& a)
{
    read(j, "Enabled", a.enabled);
    read(j, "Pitch", a.pitch);
    read(j, "Yaw", a.yaw);
    read(j, "Pitch angle", a.pitchAngle);
}

void AntiAim::fromJson(const json& j) noexcept
{
    from_json(j, config->antiAim);
}

void AntiAim::resetConfig() noexcept
{

}
