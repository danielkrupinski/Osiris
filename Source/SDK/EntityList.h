#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct EntityPOD;
struct EntityListPOD;

class EntityList : public VirtualCallableFromPOD<EntityList, EntityListPOD> {
public:
    VIRTUAL_METHOD(EntityPOD*, getEntity, 3, (int index), (index))
#if IS_WIN32()
    VIRTUAL_METHOD(EntityPOD*, getEntityFromHandle, 4, (int handle), (handle))
#else
    VIRTUAL_METHOD(EntityPOD*, getEntityFromHandle, 4, (int handle), (&handle))
#endif
    VIRTUAL_METHOD(int, getHighestEntityIndex, 6, (), ())
};

}
