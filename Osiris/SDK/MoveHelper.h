#pragma once

#include "VirtualMethod.h"

class Entity;

class MoveHelper {
public:
    VIRTUAL_METHOD(void, setHost, 1, (Entity* host), (this, host))
};
