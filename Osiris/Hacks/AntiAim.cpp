#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"

void AntiAim::run(UserCmd* cmd, const float oldYaw, bool& sendPacket) noexcept
{
	if (config.antiAim.enabled) {
		if (sendPacket && !interfaces.engine->getNetworkChannel()->chokedPackets)
			sendPacket = false;

		if (sendPacket && interfaces.engine->getNetworkChannel()->chokedPackets && !(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))) {
			auto absMaxDesyncAngle = fabsf(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle());
			if (cmd->viewangles.y - oldYaw == 0.0f)
				cmd->viewangles.y += absMaxDesyncAngle;
			else
				cmd->viewangles.y = oldYaw + std::clamp(cmd->viewangles.y - oldYaw, -absMaxDesyncAngle, absMaxDesyncAngle);
		}
	}
}