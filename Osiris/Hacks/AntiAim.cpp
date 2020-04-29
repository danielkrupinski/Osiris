#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Surface.h"
#include "../SDK/GameEvent.h"

bool LbyUpdate()
{
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    static float Update = 0.f;
    if (!(localPlayer->flags() & 1))
    {
        return false;
    }
    if (localPlayer->velocity() > 0.f)
    {
        Update = memory.globalVars->serverTime() + 0.22f;
    }
    if (Update <= memory.globalVars->serverTime())
    {
        Update = memory.globalVars->serverTime() + 1.1f;
        return true;
    }
    return false;
}

static bool inver;
void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (config.antiAim.enabled) {
        auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (!localPlayer || !localPlayer->isAlive())
            return;
        if (localPlayer->isInThrow())
            return;
        if (config.antiAim.pitch && cmd->viewangles.x == currentViewAngles.x) 
        {
            cmd->viewangles.x = config.antiAim.pitchAngle;
        }
        if (config.antiAim.yaw && cmd->viewangles.y == currentViewAngles.y)
        {
            float delta = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
            static bool invert = true;
            static float lastTime{ 0.0f };
            if (GetAsyncKeyState(config.antiAim.invert) && memory.globalVars->realtime - lastTime > 0.5f) {
                invert = !invert;
                lastTime = memory.globalVars->realtime;
            }
            inver = invert;
            if (config.antiAim.lby)
            {
                if (LbyUpdate() == true)
                {
                    sendPacket = false;
                    invert ? cmd->viewangles.y -= delta : cmd->viewangles.y += delta;
                    return;
                }
            }
            if (!sendPacket) {
                invert ? cmd->viewangles.y += delta: cmd->viewangles.y -= delta;
            }
            if(!config.antiAim.lby)
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
void AntiAim::indicators() noexcept
{
    if (config.antiAim.indicators && config.antiAim.enabled) {

        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextColor(255, 255, 255);
        auto [w, h] = interfaces.surface->getScreenSize();
        if (!inver)
        {
            interfaces.surface->setTextPosition((w / 2) - 50, (h / 2) + 5);
            interfaces.surface->printText(L"<");
        }
        else
        {
            interfaces.surface->setTextPosition((w / 2) + 50, (h / 2) + 5);
            interfaces.surface->printText(L">");
        }
    }
}
