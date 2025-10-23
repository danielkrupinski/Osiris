#pragma once

#include <Config/ConfigVariable.h>
#include "ViewmodelModParams.h"

namespace viewmodel_mod_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(ModifyFov, bool, true);
CONFIG_VARIABLE_RANGE(Fov, viewmodel_mod_params::kFov);

}
