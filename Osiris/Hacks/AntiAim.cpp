#include "AntiAim.h"
#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GlobalVars.h"

bool Lby()
{
	constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick); };
	auto serverTime = memory->globalVars->serverTime();
	static float nextLby, lastLby;

	if (!(localPlayer->flags() & PlayerFlags::ONGROUND))
	{
		return false;
	}
	if (localPlayer->velocity().notNull() > 0.1f)
	{
		nextLby = 0.22f;
		return false;
	}
	if (serverTime - lastLby - nextLby >= 0)
	{
		nextLby = 1.125f;
		lastLby = serverTime;
		return true;
	}
	else
	{
		return false;
	}
}

void AntiAim::run(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (config->antiAim.enabled) {
        if (!localPlayer)
            return;

		if (cmd->buttons & (UserCmd::IN_USE) || (localPlayer->moveType() == MoveType::LADDER) || (localPlayer->moveType() == MoveType::NOCLIP))
			return;

		float delta = localPlayer->getMaxDesyncAngle();
		static bool invert = true;
		auto realtime = memory->globalVars->realtime;
		static float lastTime{ 0.0f };

		if ((GetAsyncKeyState(config->antiAim.hotKey)) && realtime - lastTime > 0.5f) {
			invert = !invert;
			lastTime = realtime;
			config->antiAim.inver = invert;
		}
        
		if (Lby())
		{
			sendPacket = false;
			invert ? cmd->viewangles.y -= delta : cmd->viewangles.y += delta;
			return;
		}

		if (!sendPacket) {
			invert ? cmd->viewangles.y += delta : cmd->viewangles.y -= delta;
		}
    }
}
