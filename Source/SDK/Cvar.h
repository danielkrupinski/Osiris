#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

namespace csgo::pod { struct ConVar; }

class Cvar {
public:
    INCONSTRUCTIBLE(Cvar)

    VIRTUAL_METHOD(csgo::pod::ConVar*, findVar, 15, (const char* name), (this, name))
};
