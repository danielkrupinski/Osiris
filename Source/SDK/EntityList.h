#pragma once

#include "VirtualMethod.h"

class Entity;

class EntityList : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2(Entity*, getEntity, 3, (int index), (index))
#ifdef _WIN32
    VIRTUAL_METHOD2(Entity*, getEntityFromHandle, 4, (int handle), (handle))
#else
    VIRTUAL_METHOD2(Entity*, getEntityFromHandle, 4, (int handle), (&handle))
#endif
    VIRTUAL_METHOD2(int, getHighestEntityIndex, 6, (), ())
};
