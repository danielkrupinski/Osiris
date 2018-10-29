#include "xorstr.hpp"

#include "Config.h"
#include "GUI.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"

Config config{ xorstr_("Osiris.json") };
GUI gui;
Interfaces interfaces;
Memory memory;
Hooks hooks;
