#pragma once
#include <cstdint>

namespace sdk
{
	class CBaseClientState
	{
	public:
		void ForceFullUpdate()
		{
			*reinterpret_cast<int*>(std::uintptr_t(this) + 0x174) = -1;
		};
	};
}