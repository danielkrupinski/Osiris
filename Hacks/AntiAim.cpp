#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include "../Memory.h"

float AntiAim::RandomFloat(float min, float max) noexcept
{
    return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
};

void AntiAim::setPitch(float pitch, UserCmd* cmd, bool sendPacket) noexcept
{
    float newviewangle = pitch;

    cmd->viewangles.x = newviewangle;
};

bool AntiAim::LbyUpdate(Vector& previousViewAngles)
{
    if (!(localPlayer->flags() & 1))
    {
        return false;
    }
    if (localPlayer->velocity() > 0.f)
    {
        config->globals.nextLBY = config->globals.serverTime + 0.22f;
    }
    if (config->globals.nextLBY <= config->globals.serverTime)
    {
        config->globals.nextLBY = config->globals.serverTime + 1.1f;
        return true;
    }
}

void AntiAim::setYaw(float yaw, UserCmd* cmd, bool sendPacket) noexcept
{
    float newviewangle = yaw;

    if (config->antiAim.yawReal)
    {
        if (!sendPacket)
        {
            if (!GetAsyncKeyState(config->antiAim.yawInverseAngleKey))
                newviewangle += localPlayer->getMaxDesyncAngle() * config->antiAim.bodyLean / 100;
            else
                newviewangle -= localPlayer->getMaxDesyncAngle() * config->antiAim.bodyLean / 100;
        }
    }

    if (config->antiAim.LBYBreaker && config->antiAim.yawReal)
        if (!sendPacket)
            if (LbyUpdate)
                newviewangle += 180.0f;

    cmd->viewangles.y = newviewangle;
};

void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (config->antiAim.enabled && localPlayer->isAlive() && interfaces->engine->isConnected() && interfaces->engine->isInGame()) {
        auto activeWeapon = localPlayer->getActiveWeapon();
        //auto weaponClass = getWeaponClass(localPlayer->getActiveWeapon()->itemDefinitionIndex2());

        if (config->antiAim.pitch)
            setPitch(config->antiAim.pitchAngle, cmd, sendPacket);

        if (config->antiAim.yawReal) {
            if (fabsf(cmd->sidemove) < 5.0f) {
                if (cmd->buttons & UserCmd::IN_DUCK)
                    cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
                else
                    cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
            }
        }

        if (config->antiAim.yaw && !(GetAsyncKeyState(config->antiAim.yawInverseAngleKey)))
        {
            if (config->antiAim.mode == 0)
                setYaw(cmd->viewangles.y + config->antiAim.yawAngle, cmd, sendPacket);
            else if (config->antiAim.mode == 1)
                setYaw(cmd->viewangles.y + config->antiAim.yawAngle + RandomFloat(config->antiAim.jitterMin, config->antiAim.jitterMax), cmd, sendPacket);
        }
        else if (config->antiAim.yaw && GetAsyncKeyState(config->antiAim.yawInverseAngleKey))
        {
            if (config->antiAim.mode == 0)
                setYaw(cmd->viewangles.y + -config->antiAim.yawAngle, cmd, sendPacket);
            else if (config->antiAim.mode == 1)
                setYaw(cmd->viewangles.y + -config->antiAim.yawAngle + RandomFloat(-config->antiAim.jitterMin, -config->antiAim.jitterMax), cmd, sendPacket);
        }
        else if (!config->antiAim.yaw && config->antiAim.yawReal)
        {
            setYaw(cmd->viewangles.y, cmd, sendPacket);
        }
    }
}
