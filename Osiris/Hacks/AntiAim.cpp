#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "Misc.h"
bool LbyBreaker()
{
	auto local_player = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
	static float NextUpdate = 0;
	float velocity = local_player->velocity().length();
	float time = memory.globalVars->serverTime();
	if (!(local_player->flags() & 1) || (!(local_player->isAlive()))) {
		return false;
	}
	if (velocity > 0.1f) {
			NextUpdate = time + 0.22f;
	}
	if (NextUpdate <= time)
	{
		NextUpdate = time + 1.1f;
		return true;
	}

	return false;
}
void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
	float desync = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
	auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
	static float lastTime{ 0.0f };
	static bool invert;
	bool lby = LbyBreaker();
	int yaw = config.antiAim.yawangle;
	Vector oldangle = cmd->viewangles;
	if (config.antiAim.enabled) {
		if (GetAsyncKeyState(config.antiAim.invertkey) && memory.globalVars->realtime - lastTime > 0.5f) {
			invert = !invert;
			lastTime = memory.globalVars->realtime;
		}
		if (config.antiAim.pitch && cmd->viewangles.x == currentViewAngles.x) {
		cmd->viewangles.x = config.antiAim.pitchAngle;
	}
        if (config.antiAim.yaw) {
			if (lby) {
				sendPacket = false;
				invert ? cmd->viewangles.y += 118.f : cmd->viewangles.y -= 118.f;
				cmd->viewangles.y += 118.f;
		}else if (sendPacket) {
					cmd->viewangles.y += yaw;
		}
		else {
				invert ? cmd->viewangles.y +=  yaw + (desync * 2) : cmd->viewangles.y +=  yaw - (desync * 2);
				}
			}
        }
	cmd->viewangles.normalize();
	Misc::fixMovement(cmd, oldangle.y);

	cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
	cmd->viewangles.z = 0.0f;
} 
