#pragma once

#include "../Hooks.h"

#include "../SDK/AnimLayer.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"

struct PlayerHook
{
	bool isHooked;

	Entity* entity;
	VmtSwap vmt;

	void reset()
	{
		vmt.restore();
		entity = nullptr;
		isHooked = false;
	}
};

struct AnimData
{
	std::array<AnimLayer, 15> layers;
	std::array<float, 24> poses;

	bool once;
	bool sendPacket;
	bool canUpdateAnimations;

	Vector viewAngles;
	Vector sentViewAngles;
	Vector absAngle;
};

namespace Animations
{
	void hookPlayers() noexcept;
	void restorePlayers() noexcept;

	void updateData(UserCmd* cmd, bool& sendPacket) noexcept;
	void updateLocal() noexcept;
}