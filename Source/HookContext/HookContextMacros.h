#pragma once

#define GET_CONFIG_VAR(name) hookContext.config().template getVariable<name>()
#define GET_CONVAR_VALUE(name) hookContext.cvarSystem().template getConVarValue<name>()
