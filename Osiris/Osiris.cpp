#include "Config.h"
#include "GUI.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"
#include "Netvars.h"
#include "AntiDetection.h"

Config config{ "Osiris" };
GUI gui;
const Interfaces interfaces;
Memory memory;
Netvars netvars;
Hooks hooks;
AntiDetection antiDetection;
