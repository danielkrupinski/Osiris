#pragma once

#include "../SDK/UserCmd.h"

namespace PredictionSystem
{
	void StartPrediction(UserCmd* cmd) noexcept;
	void EndPrediction() noexcept;
};
