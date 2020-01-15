#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"

void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
	static bool bFlip = false;

    if (config.antiAim.enabled) {
		if (config.antiAim.pitch && cmd->viewangles.x == currentViewAngles.x)
			cmd->viewangles.x = config.antiAim.pitchAngle;

		if (config.antiAim.yaw && cmd->viewangles.y == currentViewAngles.y) {
			switch (config.antiAim.mode) {
			case 0: // legit
				if (!sendPacket) {
					cmd->viewangles.y += interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
					if (fabsf(cmd->sidemove) < 5.0f) {
						if (cmd->buttons & UserCmd::IN_DUCK)
							cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
						else
							cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
					}
				}
				break;
			case 1: // static
				cmd->viewangles.y += 180; // TODO: add yaw offset slider.
				break;
			case 2: // jitter
				cmd->viewangles.y += 180;
				cmd->viewangles.y += (bFlip) ? 20 : -20; // TODO: add yaw range slider.
				bFlip ^= 1; // equal to x = !x
				break;
			case 3: // spin
				cmd->viewangles.y = previousViewAngles.y + 45;
				break;
			}
		}
    }
}
