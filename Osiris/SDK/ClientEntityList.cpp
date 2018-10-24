#include "ClientEntityList.h"
#include "../Utils.h"

BaseEntity* ClientEntityList::getClientEntity(int index)
{
    return callVirtualFunction<BaseEntity*(__thiscall*)(void*, int)>(this, 3)(this, index);
}
