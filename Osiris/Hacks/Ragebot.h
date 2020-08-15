#pragma once


struct UserCmd;
struct Vector;

namespace Ragebot {
	void Autostop(UserCmd* cmd) noexcept;
	void run(UserCmd* cmd) noexcept;
}