#pragma once

#include <cstddef>

struct AnimState {
    std::byte pad[164];
    float duckAmount;
    std::byte pad1[80];
    float footSpeed;
    float footSpeed2;
    std::byte pad2[22];
    float stopToFullRunningFraction;
    std::byte pad3[532];
    float velocitySubtractY;
	char pad6[91];
	void* pBaseEntity;
	void* pActiveWeapon;
	void* pLastActiveWeapon;
	float m_flLastClientSideAnimationUpdateTime;
	int m_iLastClientSideAnimationUpdateFramecount;
	float m_flEyePitch;
	float m_flEyeYaw;
	float m_flPitch;
	float m_flGoalFeetYaw;
	float m_flCurrentFeetYaw;
	float m_flCurrentTorsoYaw;
	float m_flUnknownVelocityLean;
	float m_flLeanAmount;
};
