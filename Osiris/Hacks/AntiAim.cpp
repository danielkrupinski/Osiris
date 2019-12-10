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
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Input.h"
#include "../SDK/Material.h"
#include "../SDK/MaterialSystem.h"
#include "../SDK/RenderContext.h"
#include "../Interfaces.h"
#include <memory.h>

void VectorAngles(const Vector& forward, QAngle& angles)
{
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0) {
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}
void SinCos(float radians, float* sine, float* cosine)
{
	register double __cosr, __sinr;
	_asm
	{
		fld		DWORD PTR[radians]
		fsincos

		mov edx, DWORD PTR[cosine]
		mov eax, DWORD PTR[sine]

		fstp DWORD PTR[edx]
		fstp DWORD PTR[eax]
	}
	*sine = __sinr;
	*cosine = __cosr;
}

void AngleVectors(const QAngle& angles, Vector& forward)
{

	float sp, sy, cp, cy;

	SinCos(degreesToRadians(angles[0]), &sy, &cy);
	SinCos(degreesToRadians(angles[1]), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}
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
Vector CalcAngle(Vector src, Vector dst)
{
	Vector angles;
	Vector delta = src - dst;

	if (delta[1] == 0.0f && delta[0] == 0.0f)
	{
		angles[0] = (delta[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
		angles[1] = 0.0f;  //yaw left/right
	}
	else
	{
		angles[0] = atan2(-delta[2], delta.length()) * -180 / M_PI;
		angles[1] = atan2(delta[1], delta[0]) * 180 / M_PI;

		if (angles[1] > 90) angles[1] -= 180;
		else if (angles[1] < 90) angles[1] += 180;
		else if (angles[1] == 90) angles[1] = 0;
	}

	angles[2] = 0.0f;
	angles.normalize();

	return angles;
}
float VectorDistance(Vector v1, Vector v2) {
	return sqrtf(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

int distance(Vector a, Vector b) {
	double distance;

	distance = sqrt(((int)a.x - (int)b.x) * ((int)a.x - (int)b.x) +
		((int)a.y - (int)b.y) * ((int)a.y - (int)b.y) +
		((int)a.z - (int)b.z) * ((int)a.z - (int)b.z));

	return (int)abs(round(distance));
}
bool LbyBreaker()
{
	const auto local_player = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
	static float NextUpdate = 0;
	auto velocity = local_player->velocity().length();
	auto currenttime = memory.globalVars->serverTime();
	if (!(local_player->flags() & 1) || (!(local_player->isAlive()))) {
		return false;
	}
	if ((local_player->flags() & 1) || (fabsf(local_player->getAnimstate()->flUpVelocity) > 99.9f)) {
		NextUpdate = currenttime + 0.22f;
	}

	if (NextUpdate <= currenttime)
	{
		NextUpdate = currenttime + 1.1f;
		return true;
	}

	return false;
}

void AntiAim::run(UserCmd* cmd, const Vector& previousViewAngles, const Vector& currentViewAngles, bool& sendPacket) noexcept
{
	const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
	auto activeWeapon = localPlayer->getActiveWeapon();
	static bool leftdesync = true;
	static float lastTime{ 0.0f };
	Vector vOldAngles = cmd->viewangles;
	float fOldForward = cmd->forwardmove;
	float fOldSidemove = cmd->sidemove;
	auto desync = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getMaxDesyncAngle();
	int yaw = config.antiAim.yawAngle;
	if (config.antiAim.enabled) {
		if (config.antiAim.pitch && cmd->viewangles.x == currentViewAngles.x) {
			cmd->viewangles.x = config.antiAim.pitchAngle;
		}
		if (GetAsyncKeyState(config.antiAim.desyncinvert) && memory.globalVars->realtime - lastTime > 0.5f)
		{
			leftdesync = !leftdesync;
			lastTime = memory.globalVars->realtime;
		}
		if (config.antiAim.yaw) {
			if (sendPacket) {
				cmd->viewangles.y += yaw;
			}
			else {
				leftdesync ? cmd->viewangles.y += desync + yaw : cmd->viewangles.y -= desync + yaw;
			}
		}
	}
	cmd->viewangles.normalize();
	cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
	cmd->viewangles.z = 0.0f;
	CorrectMovement(vOldAngles, cmd, fOldForward, fOldSidemove);

}
void AntiAim::type(UserCmd* cmd, bool& sendPacket)  noexcept
{
	const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
	static bool switch_ = true;
	static bool left = true;
	static float lastTime{ 0.0f };
	bool breaklby = LbyBreaker();
	if (config.antiAim.enabled) {
		if (GetAsyncKeyState(config.antiAim.desyncinvert) && memory.globalVars->realtime - lastTime > 0.5f)
		{
			left = !left;
			lastTime = memory.globalVars->realtime;
		}
		if (config.antiAim.type == 1) {
			if (fabsf(cmd->sidemove) < 5.0f) {
				if (cmd->buttons & UserCmd::IN_DUCK)
					cmd->sidemove = cmd->tickCount & 1 ? 3.25f : -3.25f;
				else
					cmd->sidemove = cmd->tickCount & 1 ? 1.1f : -1.1f;
			}
		}
		if (config.antiAim.type == 2) {
			if (breaklby) {
				sendPacket = false;
				cmd->viewangles.y -= 180.f;
			}
		}
	}
	cmd->viewangles.normalize();
	cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
	cmd->viewangles.z = 0.0f;
}