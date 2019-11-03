#include "AntiAim.h"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
#include <cmath>
#include "Misc.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Surface.h"
void CorrectMovement(Vector vOldAngles, UserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	// side/forward move correction
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(degreesToRadians(deltaView)) * fOldForward + cos(degreesToRadians(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(degreesToRadians(deltaView)) * fOldForward + sin(degreesToRadians(deltaView + 90.f)) * fOldSidemove;
}

void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
	const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
	const auto activeWeapon = localPlayer->getActiveWeapon();
	static bool leftdesync = true;
	Vector vOldAngles = cmd->viewangles;
	float fOldForward = cmd->forwardmove;

	float fOldSidemove = cmd->sidemove;

	if (config.antiAim.enabled) {
		if (config.antiAim.pitch && cmd->viewangles.x == currentViewAngles.x) {
			cmd->viewangles.x = config.antiAim.pitchAngle;
		}
		if (GetAsyncKeyState(config.antiAim.desyncleft) & 1)
		{
			leftdesync = false;
		}
		if (GetAsyncKeyState(config.antiAim.desyncright) & 1) {
			leftdesync = true;
		}
		if (config.antiAim.yaw) {

			if (!sendPacket) {
				cmd->viewangles.y += 180.f;
			}
			if (sendPacket) {
				leftdesync ? cmd->viewangles.y += interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle() : cmd->viewangles.y -= interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
			}
			if (fabsf(cmd->sidemove) < 5.0f) {
				if (cmd->buttons & UserCmd::IN_DUCK)
					cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
				else
					cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
			}
		}
		if (config.antiAim.legit) {

			if (!sendPacket) {
				cmd->viewangles.y += 0;
			}
			if (sendPacket) {
				leftdesync ? cmd->viewangles.y += interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle() : cmd->viewangles.y -= interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
			}
			if (fabsf(cmd->sidemove) < 5.0f) {
				if (cmd->buttons & UserCmd::IN_DUCK)
					cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
				else
					cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
			}
		}
	}
	CorrectMovement(vOldAngles, cmd, fOldForward, fOldSidemove);
}
