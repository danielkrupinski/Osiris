#pragma once

#include "Entity.h"

class MoveHelper
{
public:
	constexpr void SetHost(Entity* host) noexcept
	{
		return callVirtualMethod<void, Entity*>(this, 1, host);
	}

	constexpr void ProcessImpacts() noexcept
	{
		return callVirtualMethod<void>(this, 4);
	}
};