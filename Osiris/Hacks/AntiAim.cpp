#define _CRT_RAND_S

#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include "../Memory.h"
#include "../SDK/Cvar.h"
#include <stdlib.h>

float AntiAim::RandomFloat(float min, float max) noexcept
{
    return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
};

bool AntiAim::LbyUpdate()
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / config->globals.tickRate); };

    if (config->antiAim.general.fakeWalk.keyToggled)
    {
        if (interfaces->engine->getNetworkChannel()->chokedPackets == config->antiAim.general.fakeWalk.maxChoke)
            return true;
        else
            return false;
    }
    if (!(localPlayer->flags() & 1))
    {
        return false;
    }
    if (localPlayer->velocity().x > 0.f || localPlayer->velocity().y > 0.f || localPlayer->velocity().z > 0.f)
    {
        config->globals.nextLBY = 0.22f;
        return false;
    }
    if (config->globals.serverTime - config->globals.lastLBY - config->globals.nextLBY >= 0)
    {
        config->globals.nextLBY = 1.1f;
        config->globals.lastLBY = config->globals.serverTime;
        return true;
    }
    else
    {
        return false;
    }
}


void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (!localPlayer)
        return;

	auto weapon = localPlayer->getActiveWeapon();

    if (!weapon)
        return;

    auto weaponClass = getWeaponClass(localPlayer->getActiveWeapon()->itemDefinitionIndex2());

    if (!weaponClass)
        return;

    if (weaponClass != (40 | 39 | 9 | 8 ) && cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))
        return;
	
    if (localPlayer->throwing(cmd))
        return;
	
   if (config->antiAim.general.yawInverseAngleKey != 0) {
        if (config->antiAim.general.yawInverseKeyMode == 0) {
            if (!GetAsyncKeyState(config->antiAim.general.yawInverseAngleKey))
            {
                config->antiAim.general.yawInversed = false;
            }
            else
                config->antiAim.general.yawInversed = true;
        }
        else {
            if (GetAsyncKeyState(config->antiAim.general.yawInverseAngleKey) & 1)
                config->antiAim.general.yawInversed = !config->antiAim.general.yawInversed;
        }
    }

    if (cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2 | UserCmd::IN_USE) || (localPlayer->moveType() == MoveType::LADDER) || (localPlayer->moveType() == MoveType::NOCLIP) || !localPlayer->isAlive())
        return;

    if (config->antiAim.standing.yaw.desync.enabled || config->antiAim.moving.yaw.desync.enabled || config->antiAim.inAir.yaw.desync.enabled)
        if (fabsf(cmd->sidemove) < 5.0f) {
            if (cmd->buttons & UserCmd::IN_DUCK)
                cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
            else
                cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
        }

    if (config->antiAim.standing.enabled && localPlayer->isAlive() && interfaces->engine->isConnected() && interfaces->engine->isInGame() && localPlayer->flags() & 1 && fabsf(cmd->sidemove) < 5.0f && fabsf(cmd->forwardmove) < 5.0f && fabsf(cmd->sidemove) > -5.0f && fabsf(cmd->forwardmove) > -5.0f)
    {
        if (config->antiAim.standing.pitch.enabled)
            cmd->viewangles.x = config->antiAim.standing.pitch.angle;

        if (config->antiAim.standing.yaw.enabled)
        {

            if (!sendPacket && config->antiAim.standing.yaw.desync.enabled)
            {
                if (config->antiAim.standing.yaw.desync.mode == 0)
                    if (config->antiAim.general.yawInversed)
                        cmd->viewangles.y = cmd->viewangles.y + -config->antiAim.standing.yaw.angle + -(localPlayer->getMaxDesyncAngle() * config->antiAim.standing.yaw.desync.bodyLean / 100);
                    else
                        cmd->viewangles.y = cmd->viewangles.y + config->antiAim.standing.yaw.angle + (localPlayer->getMaxDesyncAngle() * config->antiAim.standing.yaw.desync.bodyLean / 100);
                else if (config->antiAim.standing.yaw.desync.mode == 1)
                {
                    float jitter = RandomFloat(config->antiAim.standing.yaw.desync.jitterMin, config->antiAim.standing.yaw.desync.jitterMax);
                    float minJitter = config->antiAim.standing.yaw.desync.jitterMin;
                    float maxJitter = config->antiAim.standing.yaw.desync.jitterMax;

                    if (minJitter < 0)
                        minJitter = -minJitter;

                    if (maxJitter < 0)
                        maxJitter = -maxJitter;

                    if (config->antiAim.standing.yaw.desync.step > (maxJitter || minJitter))
                        if (-minJitter > maxJitter)
                            std::clamp(jitter, maxJitter, -minJitter);
                        else
                            std::clamp(jitter, -minJitter, maxJitter);
                    else
                        std::clamp(jitter, -config->antiAim.standing.yaw.desync.step, config->antiAim.standing.yaw.desync.step);

                    jitter = jitter * localPlayer->getMaxDesyncAngle() / 100;

                    if (config->antiAim.general.yawInversed)
                        jitter = -jitter;

                    if (LbyUpdate() && config->antiAim.standing.yaw.desync.LBYBreaker.enabled)
                        if (config->antiAim.general.yawInversed)
                            cmd->viewangles.y += -config->antiAim.standing.yaw.desync.LBYBreaker.angle;
                        else
                            cmd->viewangles.y += -config->antiAim.standing.yaw.desync.LBYBreaker.angle;
                    else
                        if (config->antiAim.general.yawInversed)
                            cmd->viewangles.y += -config->antiAim.standing.yaw.angle + jitter;
                        else
                            cmd->viewangles.y += config->antiAim.standing.yaw.angle + jitter;
                }
            }
            else if (sendPacket || !config->antiAim.standing.yaw.desync.enabled)
            {
                if (!config->antiAim.standing.yaw.fake.mode)
                    cmd->viewangles.y += config->antiAim.standing.yaw.angle;
                else if (config->antiAim.standing.yaw.fake.mode == 1)
                {
                    float jitter = RandomFloat(config->antiAim.standing.yaw.fake.jitterMin, config->antiAim.standing.yaw.fake.jitterMax);
                    float minJitter = config->antiAim.standing.yaw.fake.jitterMin;
                    float maxJiter = config->antiAim.standing.yaw.fake.jitterMax;

                    if (minJitter < 0)
                        minJitter = -minJitter;

                    if (maxJiter < 0)
                        maxJiter = -maxJiter;

                    if (config->antiAim.standing.yaw.fake.step > (maxJiter || minJitter))
                        std::clamp(jitter, -minJitter, maxJiter);
                    else
                        std::clamp(jitter, -config->antiAim.standing.yaw.fake.step, config->antiAim.standing.yaw.fake.step);

                    if (config->antiAim.general.yawInversed)
                        cmd->viewangles.y += -config->antiAim.standing.yaw.angle + -jitter;
                    else
                        cmd->viewangles.y += config->antiAim.standing.yaw.angle + jitter;
                }
            }
        }
    }
    else if (config->antiAim.moving.enabled && localPlayer->isAlive() && interfaces->engine->isConnected() && interfaces->engine->isInGame() && (localPlayer->velocity().x < 5.0f || localPlayer->velocity().y < 5.0f || localPlayer->velocity().z < 5.0f))
    {
        if (config->antiAim.moving.pitch.enabled)
            cmd->viewangles.x = config->antiAim.moving.pitch.angle;

        if (config->antiAim.moving.yaw.enabled)
        {
            if (!sendPacket && config->antiAim.moving.yaw.desync.enabled)
            {
                if (config->antiAim.moving.yaw.desync.mode == 0)
                    if (config->antiAim.general.yawInversed)
                        cmd->viewangles.y = -cmd->viewangles.y + -config->antiAim.moving.yaw.angle + (localPlayer->getMaxDesyncAngle() * config->antiAim.moving.yaw.desync.bodyLean / 100);
                    else
                        cmd->viewangles.y = cmd->viewangles.y + config->antiAim.moving.yaw.angle + (localPlayer->getMaxDesyncAngle() * config->antiAim.moving.yaw.desync.bodyLean / 100);
                else if (config->antiAim.moving.yaw.desync.mode == 1)
                {
                    float jitter = RandomFloat(config->antiAim.moving.yaw.desync.jitterMin, config->antiAim.moving.yaw.desync.jitterMax);
                    float minJitter = config->antiAim.moving.yaw.desync.jitterMin;
                    float maxJitter = config->antiAim.moving.yaw.desync.jitterMax;

                    if (minJitter < 0)
                        minJitter = -minJitter;

                    if (maxJitter < 0)
                        maxJitter = -maxJitter;

                	    if (config->antiAim.moving.yaw.desync.step > (maxJitter || minJitter))
                        if (-minJitter > maxJitter)
                            std::clamp(jitter, maxJitter, -minJitter);
                        else
                            std::clamp(jitter, -minJitter, maxJitter);
                    else
                        std::clamp(jitter, -config->antiAim.moving.yaw.desync.step, config->antiAim.moving.yaw.desync.step);

                	   jitter = jitter * localPlayer->getMaxDesyncAngle() / 100;

                    if (config->antiAim.general.yawInversed)
                        jitter = -jitter;

                    if (LbyUpdate() && config->antiAim.moving.yaw.desync.LBYBreaker.enabled)
                        if (config->antiAim.general.yawInversed)
                            cmd->viewangles.y += -config->antiAim.moving.yaw.desync.LBYBreaker.angle;
                        else
                            cmd->viewangles.y += config->antiAim.moving.yaw.desync.LBYBreaker.angle;
                    else
                        if (config->antiAim.general.yawInversed)
                            cmd->viewangles.y += -config->antiAim.moving.yaw.angle + jitter;
                        else
                            cmd->viewangles.y += config->antiAim.moving.yaw.angle + jitter;
                }
            }
            else if (sendPacket || !config->antiAim.moving.yaw.desync.enabled)
            {
                if (config->antiAim.moving.yaw.fake.mode == 0)
                    cmd->viewangles.y += config->antiAim.moving.yaw.angle;
                else if (config->antiAim.moving.yaw.fake.mode == 1)
                {
                    float jitter = RandomFloat(config->antiAim.moving.yaw.fake.jitterMin, config->antiAim.moving.yaw.fake.jitterMax);
                    float minJitter = config->antiAim.moving.yaw.fake.jitterMin;
                    float maxJitter = config->antiAim.moving.yaw.fake.jitterMax;

                    if (minJitter < 0)
                        minJitter = -minJitter;

                    if (maxJitter < 0)
                        maxJitter = -maxJitter;

                    if (config->antiAim.moving.yaw.fake.step > (maxJitter || minJitter))
                        if (-minJitter > maxJitter)
                            std::clamp(jitter, maxJitter, -minJitter);
                        else
                            std::clamp(jitter, -minJitter, maxJitter);
                    else
                        std::clamp(jitter, -config->antiAim.moving.yaw.fake.step, config->antiAim.moving.yaw.fake.step);

                    if (config->antiAim.general.yawInversed)
                        cmd->viewangles.y += -config->antiAim.moving.yaw.angle + -jitter;
                    else
                        cmd->viewangles.y += config->antiAim.moving.yaw.angle + jitter;
                }
            }
        }
    }
    else if (config->antiAim.inAir.enabled && localPlayer->isAlive() && interfaces->engine->isConnected() && interfaces->engine->isInGame() && !(localPlayer->flags() & 1))
    {
        if (config->antiAim.inAir.pitch.enabled)
            cmd->viewangles.x = config->antiAim.inAir.pitch.angle;

        if (config->antiAim.inAir.yaw.enabled)
        {
            if (!sendPacket && config->antiAim.inAir.yaw.desync.enabled)
            {
                if (config->antiAim.inAir.yaw.desync.mode == 0)
                    if (config->antiAim.general.yawInversed)
                        cmd->viewangles.y = cmd->viewangles.y + -config->antiAim.inAir.yaw.angle + -(localPlayer->getMaxDesyncAngle() * config->antiAim.inAir.yaw.desync.bodyLean / 100);
                    else
                        cmd->viewangles.y = cmd->viewangles.y + config->antiAim.inAir.yaw.angle + (localPlayer->getMaxDesyncAngle() * config->antiAim.inAir.yaw.desync.bodyLean / 100);
                else if (config->antiAim.inAir.yaw.desync.mode == 1)
                {
                    float jitter = RandomFloat(config->antiAim.inAir.yaw.desync.jitterMin, config->antiAim.inAir.yaw.desync.jitterMax);
                    float minJitter = config->antiAim.inAir.yaw.desync.jitterMin;
                    float maxJiter = config->antiAim.inAir.yaw.desync.jitterMax;

                    if (minJitter < 0)
                        minJitter = -minJitter;

                    if (maxJiter < 0)
                        maxJiter = -maxJiter;

                    if (config->antiAim.inAir.yaw.desync.step > (maxJiter || minJitter))
                        std::clamp(jitter, -minJitter, maxJiter);
                    else
                        std::clamp(jitter, -config->antiAim.inAir.yaw.desync.step, config->antiAim.inAir.yaw.desync.step);

                    jitter = jitter * localPlayer->getMaxDesyncAngle() / 100;

                    if (config->antiAim.general.yawInversed)
                        jitter = -jitter;

                    if (LbyUpdate() && config->antiAim.inAir.yaw.desync.LBYBreaker.enabled)
                        if (config->antiAim.general.yawInversed)
                            cmd->viewangles.y += -config->antiAim.inAir.yaw.desync.LBYBreaker.angle;
                        else
                            cmd->viewangles.y += config->antiAim.inAir.yaw.desync.LBYBreaker.angle;
                    else
                        if (config->antiAim.general.yawInversed)
                            cmd->viewangles.y += -config->antiAim.inAir.yaw.angle + jitter;
                        else
                            cmd->viewangles.y += -config->antiAim.inAir.yaw.angle + jitter;
                }
            }
            else if (sendPacket || !config->antiAim.inAir.yaw.desync.enabled)
            {
                if (!config->antiAim.inAir.yaw.fake.mode)
                    cmd->viewangles.y += config->antiAim.inAir.yaw.angle;
                else if (config->antiAim.inAir.yaw.fake.mode == 1)
                {
                    float jitter = RandomFloat(config->antiAim.inAir.yaw.fake.jitterMin, config->antiAim.inAir.yaw.fake.jitterMax);
                    float minJitter = config->antiAim.inAir.yaw.fake.jitterMin;
                    float maxJiter = config->antiAim.inAir.yaw.fake.jitterMax;

                    if (minJitter < 0)
                        minJitter = -minJitter;

                    if (maxJiter < 0)
                        maxJiter = -maxJiter;

                    if (config->antiAim.inAir.yaw.fake.step > (maxJiter || minJitter))
                        std::clamp(jitter, -minJitter, maxJiter);
                    else
                        std::clamp(jitter, -config->antiAim.inAir.yaw.fake.step, config->antiAim.inAir.yaw.fake.step);

                    if (config->antiAim.general.yawInversed)
                        cmd->viewangles.y += -config->antiAim.inAir.yaw.angle + -jitter;
                    else
                        cmd->viewangles.y += config->antiAim.inAir.yaw.angle + jitter;
                }
            }
        }
    }
}

void AntiAim::fakeWalk(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (config->antiAim.general.fakeWalk.key != 0) {
        if (config->antiAim.general.fakeWalk.keyMode == 0) {
            if (!GetAsyncKeyState(config->antiAim.general.fakeWalk.key))
            {
                config->antiAim.general.fakeWalk.keyToggled = false;
            }
            else
                config->antiAim.general.fakeWalk.keyToggled = true;
        }
        else {
            if (GetAsyncKeyState(config->antiAim.general.fakeWalk.key) & 1)
                config->antiAim.general.fakeWalk.keyToggled = !config->antiAim.general.fakeWalk.keyToggled;
        }
    }

    if (config->antiAim.general.fakeWalk.enabled && config->antiAim.general.fakeWalk.keyToggled)
    {
        if (interfaces->engine->getNetworkChannel()->chokedPackets < config->antiAim.general.fakeWalk.maxChoke)
        {
            sendPacket = false;
        }
        else if (interfaces->engine->getNetworkChannel()->chokedPackets == config->antiAim.general.fakeWalk.maxChoke)
        {
            sendPacket = false;
        }
        else if (interfaces->engine->getNetworkChannel()->chokedPackets == config->antiAim.general.fakeWalk.maxChoke + 1)
        {
            cmd->forwardmove = 0;

            if (cmd->buttons & UserCmd::IN_DUCK)
                cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
            else
                cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;

            sendPacket = false;
        }
        else
        {
            cmd->forwardmove = 0;

            if (cmd->buttons & UserCmd::IN_DUCK)
                cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
            else
                cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;

            sendPacket = true;
        }
    }
}
