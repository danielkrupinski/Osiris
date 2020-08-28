#pragma once

struct UserCmd;

namespace Tickbase
{
	void shiftTicks(int, UserCmd*, bool = false) noexcept;
	void run(UserCmd*) noexcept;
	bool canShift(int ticks, bool shiftanyways) noexcept;

	struct Tick
	{
		int	maxUsercmdProcessticks{ 17 }; //on valve servers this is 8 ticks, always do +1 command
		int ticksAllowedForProcessing{ maxUsercmdProcessticks };
		int chokedPackets{ 0 };
		int fakeLag{ 0 };
		int tickshift{ 0 };
		int tickbase{ 0 };
		int commandNumber{ 0 };
		int ticks {0};
	};
	inline std::unique_ptr<Tick> tick;
}