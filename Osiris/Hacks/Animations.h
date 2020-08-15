#pragma once
#include "../SDK/matrix3x4.h"
#include "../SDK/Vector.h"

struct UserCmd;

namespace Animations
{
	void update(UserCmd*, bool& sendPacket) noexcept;
	void real() noexcept;
	void fake() noexcept;
	struct Datas
	{
		bool sendPacket;
		bool gotMatrix;
		Vector viewangles;
		matrix3x4 fakematrix[256];
	};
	extern Datas data;
} 