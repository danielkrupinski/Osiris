#pragma once

#include "VirtualMethod.h"

namespace csgo::pod { struct Entity; }
namespace csgo::pod { struct EntityList; }

class EntityList : public VirtualCallableFromPOD<EntityList, csgo::pod::EntityList> {
public:
    VIRTUAL_METHOD(csgo::pod::Entity*, getEntity, 3, (int index), (index))
#if IS_WIN32()
    VIRTUAL_METHOD(csgo::pod::Entity*, getEntityFromHandle, 4, (int handle), (handle))
#else
    VIRTUAL_METHOD(csgo::pod::Entity*, getEntityFromHandle, 4, (int handle), (&handle))
#endif
    VIRTUAL_METHOD(int, getHighestEntityIndex, 6, (), ())
};
