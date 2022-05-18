#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Entity;

class EntityList {
public:
    INCONSTRUCTIBLE(EntityList)

    VIRTUAL_METHOD(Entity*, getEntity, 3, (int index), (this, index))
#ifdef _WIN32
    VIRTUAL_METHOD(Entity*, getEntityFromHandle, 4, (int handle), (this, handle))
#else
    VIRTUAL_METHOD(Entity*, getEntityFromHandle, 4, (int handle), (this, &handle))
#endif
    VIRTUAL_METHOD(int, getHighestEntityIndex, 6, (), (this))
};
