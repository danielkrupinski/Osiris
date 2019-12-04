#pragma once

#include <cstddef>
#include "Vector.h"

struct ViewSetup {
	std::byte pad[176];
	float fov;
	float fovViewModel;
	Vector origin;
	std::byte pad1[16];
	float farZ;
};