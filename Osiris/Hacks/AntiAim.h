#pragma once

struct UserCmd;
struct Vector;

namespace AntiAim {
	float RandomFloat(float min, float max) noexcept;
	void setPitch(float pitch, UserCmd* cmd, bool sendPacket) noexcept;
	bool LbyUpdate();
	void setYaw(float yaw, float desyncYaw, UserCmd* cmd, bool sendPacket) noexcept;
	void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;
}
