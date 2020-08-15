#pragma once

struct UserCmd;
struct Vector;

namespace AntiAim {
    float RandomFloat(float min, float max) noexcept;
	bool LbyUpdate();
	void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;
	void fakeWalk(UserCmd* cmd, bool& sendPacket) noexcept;
}
