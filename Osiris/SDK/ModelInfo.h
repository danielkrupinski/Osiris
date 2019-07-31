#pragma once

#include "Utils.h"

class ModelInfo final {
public:
	constexpr auto getModelIndex(const char* name) noexcept
	{
		return callVirtualMethod<int, const char*>(this, 2, name);
	}
};
