#include <cmath>

#include "AntiAim.h"

#include "../imgui/imgui.h"

#include "../ConfigStructs.h"
#include "../SDK/Entity.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/GameEvent.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Surface.h"
#include <imguiCustom.h>
#include <Interfaces.h>




#if OSIRIS_ANTIAIM()

struct AntiAimConfig {
    bool enabled = false;
    bool pitch = false;
    bool yaw = false;
    bool lby = false;
    bool indicators = false;
    bool swayy = false;
    KeyBind invert;
    float pitchAngle = 0.0f;
} antiAimConfig;

static float desyncDelta{ 0 };
static bool flipLby{ false };
void AntiAim::frozenaa(GameEvent* event) noexcept
{
    static std::mutex mtx;
    std::scoped_lock _{ mtx };

    struct PlayerPurchases {
        int totalCost;
        std::unordered_map<std::string, int> items;
    };

    static std::unordered_map<int, PlayerPurchases> playerPurchases;
    static std::unordered_map<std::string, int> purchaseTotal;
    static int totalCost;

    static auto freezeEnd = 0.0f;

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("round_freeze_end"): {
            frozen = false;
            break;
        }
        case fnv::hash("round_prestart"):
            frozen = true;
            break;
        case fnv::hash("round_end"):
            frozen = true;
            break;
        }
    }


}

bool LbyUpdate()
{
    const auto velocity = localPlayer->velocity();
    const auto speed = velocity.length2D();
    static float Update = 0.f;
    if (!(localPlayer->flags() & 1))
    {
        return false;
    }
    if (speed > 0.1f || fabsf(localPlayer->getAnimstate()->UpVelocity) > 100.f)
    {
        Update = memory->globalVars->serverTime() + 0.22f;
    }
    if (Update <= memory->globalVars->serverTime())
    {
        Update = memory->globalVars->serverTime() + 1.1f;
        return true;
    }
    return false;
}


void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (antiAimConfig.enabled) {
        if (!localPlayer || !localPlayer->isAlive())
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
        if (frozen)
            return;
        indicatorsa = antiAimConfig.indicators;
        if (antiAimConfig.pitch && cmd->viewangles.x == currentViewAngles.x) {
            cmd->viewangles.x = antiAimConfig.pitchAngle;
        }
        cmd->buttons &= ~(UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVERIGHT | UserCmd::IN_MOVELEFT);
        static float deltaMultiplier{ localPlayer->getMaxDesyncAngle() / 58.f };
        if (antiAimConfig.yaw && cmd->viewangles.y == currentViewAngles.y) {
            static float sent = 0.f;
            if (sendPacket) {
                cmd->viewangles.y += 0.f;
                sent = cmd->viewangles.y;
            }
            //float delta = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
            float delta = localPlayer->getMaxDesyncAngle();

            if (antiAimConfig.invert.isPressed()) {
                invertw = !invertw;
               
            }
            if (antiAimConfig.lby) //todo: add jitter using randomfloat from above
            {
                //float delta = localPlayer->getMaxDesyncAngle();
                float delta = 119.95f;
                
                
                 if (LbyUpdate() == true)
                 {
                     sendPacket = false;
                  if (!antiAimConfig.swayy)
                  {
                    if (!invertw)
                    {cmd->viewangles.y = sent + delta;}
                    else
                    {cmd->viewangles.y = sent - delta;}
                    return;
                  }
                 
                    else {
                        
                      if (!invertw)
                      {
                          cmd->viewangles.y = sent + delta;
                          invertw = true;
                      }
                      else
                      {
                          cmd->viewangles.y = sent - delta;
                          invertw = false;
                      }
                      return;
                        
                    }
                 }
            }
            desyncDelta = (58.f * 2) * deltaMultiplier;
            if (!sendPacket) {
                if (!invertw)
                {
                    cmd->viewangles.y = sent + desyncDelta;
                }
                else
                {
                    cmd->viewangles.y = sent - desyncDelta;
                }
                
            }
            if (!antiAimConfig.lby)
            {
                if (fabsf(cmd->sidemove) < 5.0f) {
                    if (cmd->buttons & UserCmd::IN_DUCK)
                        cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
                    else
                        cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
                }
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
    ImGui::Checkbox("##pitch", &antiAimConfig.pitch);
    ImGui::SameLine();
    ImGui::SliderFloat("Pitch", &antiAimConfig.pitchAngle, -89.0f, 89.0f, "%.2f");
    ImGui::Checkbox("Yaw", &antiAimConfig.yaw);
    if (antiAimConfig.yaw) {
        ImGui::Checkbox("Lby Breaker", &antiAimConfig.lby);
    }
    ImGui::Checkbox("Indicators", &antiAimConfig.indicators);
    ImGui::Text("Invert AA");
    ImGui::SameLine();
    ImGui::hotkey("key", antiAimConfig.invert);
    ImGui::Text("Sway");
    ImGui::SameLine();
    ImGui::Checkbox(" ", &antiAimConfig.swayy);
    if (!contentOnly)
        ImGui::End();
}

static void to_json(json& j, const AntiAimConfig& o, const AntiAimConfig& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Pitch", pitch);
    WRITE("Pitch angle", pitchAngle);
    WRITE("Yaw", yaw);
    WRITE("Lby", lby);
    WRITE("Indicators", indicators);
    WRITE("Invert", invert);
    WRITE("Swayy", swayy);


    
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
    read(j, "Pitch angle", a.pitchAngle);
    read(j, "Lby", a.lby);
    read(j, "Indicators", a.indicators);
    read(j, "Invert", a.invert);
    read(j, "Swayy", a.swayy);

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
