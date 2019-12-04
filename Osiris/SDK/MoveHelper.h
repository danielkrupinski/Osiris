#pragma once

#include "Entity.h"

class MoveHelper
{
public:
	constexpr void SetHost(Entity* host)
	{
		return callVirtualMethod<void, Entity*>(this, 1, host);
	}
};