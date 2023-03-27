#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct EntityPOD;
struct EntityListPOD;

struct EntityList : GameClass<EntityList, EntityListPOD> {
    VIRTUAL_METHOD(EntityPOD*, getEntity, 3, (int index), (index))
#if IS_WIN32()
    VIRTUAL_METHOD(EntityPOD*, getEntityFromHandle, 4, (int handle), (handle))
#else
    VIRTUAL_METHOD(EntityPOD*, getEntityFromHandle, 4, (int handle), (&handle))
#endif
    VIRTUAL_METHOD(int, getHighestEntityIndex, 6, (), ())
};

}
