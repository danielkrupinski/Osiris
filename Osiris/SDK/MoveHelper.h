#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Entity;

class MoveHelper {
public:
    INCONSTRUCTIBLE(MoveHelper)

    VIRTUAL_METHOD(void, setHost, 1, (Entity* host), (this, host))
};
