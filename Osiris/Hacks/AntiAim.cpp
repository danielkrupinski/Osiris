#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#define square1( x ) ( x * x )

inline float FastSqrt1(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}



void SlowWalk(UserCmd* get_cmd, float get_speed)
{
	if (get_speed <= 0.f)
		return;

	float min_speed = (float)(FastSqrt1(square1(get_cmd->forwardmove) + square1(get_cmd->sidemove) + square1(get_cmd->upmove)));
	if (min_speed <= 0.f)
		return;

	get_cmd->buttons &= ~IN_WALK;

	if (get_cmd->buttons & UserCmd::IN_DUCK)
	{
		const auto state = localPlayer->getAnimstate();

		if (state)
			get_speed *= 2.94117647f - (2.94117647f * state->duckAmount);
		else
			get_speed *= 2.94117647f;

	}

	if (min_speed <= get_speed)
		return;

	float speed = get_speed / min_speed;

	get_cmd->forwardmove *= speed;
	get_cmd->sidemove *= speed;
	get_cmd->upmove *= speed;
}


void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
    if (config->antiAim.Slowwalk && GetAsyncKeyState(config->antiAim.slowwalkkey))
	{
		SlowWalk(cmd, config->antiAim.slowwalkspeed);
	}
    
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
