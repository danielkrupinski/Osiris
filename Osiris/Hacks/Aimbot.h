#pragma once

#include "../SDK/UserCmd.h"

namespace Aimbot {
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
	void autoZeus(UserCmd* cmd) noexcept;
	void run(UserCmd*) noexcept;
}
