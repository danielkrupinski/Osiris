#pragma once

#include <cstdint>

#include <Config/ConfigVariable.h>

namespace trigger_bot_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(TeamCheck, bool, true);
CONFIG_VARIABLE(DelayMin, std::uint16_t, 10);
CONFIG_VARIABLE(DelayMax, std::uint16_t, 25);
CONFIG_VARIABLE(ShotDuration, std::uint16_t, 50);

}
