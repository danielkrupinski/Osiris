#pragma once

#include "VirtualMethod.h"

class EntityList : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2(std::uintptr_t, getEntity, 3, (int index), (index))
#if IS_WIN32()
    VIRTUAL_METHOD2(std::uintptr_t, getEntityFromHandle, 4, (int handle), (handle))
#else
    VIRTUAL_METHOD2(std::uintptr_t, getEntityFromHandle, 4, (int handle), (&handle))
#endif
    VIRTUAL_METHOD2(int, getHighestEntityIndex, 6, (), ())
};
