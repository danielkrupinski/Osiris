#pragma once

#include "VirtualMethod.h"

namespace csgo::pod { struct Entity; }

class EntityList : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD(csgo::pod::Entity*, getEntity, 3, (int index), (index))
#if IS_WIN32()
    VIRTUAL_METHOD(csgo::pod::Entity*, getEntityFromHandle, 4, (int handle), (handle))
#else
    VIRTUAL_METHOD(csgo::pod::Entity*, getEntityFromHandle, 4, (int handle), (&handle))
#endif
    VIRTUAL_METHOD(int, getHighestEntityIndex, 6, (), ())
};
