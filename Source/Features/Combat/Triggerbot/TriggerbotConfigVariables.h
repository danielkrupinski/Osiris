#pragma once

#include <Config/ConfigVariable.h>

namespace triggerbot_vars
{
    CONFIG_VARIABLE(Enabled, bool, false);
    CONFIG_VARIABLE(VisibilityCheck, bool, true);
    CONFIG_VARIABLE(FlashImmunity, bool, true);
}
