#pragma once

#define GET_CONFIG_VAR(name) hookContext.config().template getVariable<name>()
#define SET_CONFIG_VAR(name, value) hookContext.config().template setVariable<name>(value)
#define GET_CONVAR_VALUE(name) hookContext.cvarSystem().template getConVarValue<name>()
