#pragma once

struct UserCmd;

namespace AntiAim {
	void run(UserCmd*, const float, bool&) noexcept;
}
