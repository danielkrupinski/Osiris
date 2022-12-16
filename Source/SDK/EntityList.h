#pragma once

#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct Entity; }
namespace pod { struct EntityList; }

class EntityList : public VirtualCallableFromPOD<EntityList, pod::EntityList> {
public:
    VIRTUAL_METHOD(pod::Entity*, getEntity, 3, (int index), (index))
#if IS_WIN32()
    VIRTUAL_METHOD(pod::Entity*, getEntityFromHandle, 4, (int handle), (handle))
#else
    VIRTUAL_METHOD(pod::Entity*, getEntityFromHandle, 4, (int handle), (&handle))
#endif
    VIRTUAL_METHOD(int, getHighestEntityIndex, 6, (), ())
};

}
