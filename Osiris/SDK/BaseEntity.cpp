#include "BaseEntity.h"

int* BaseEntity::getFlags()
{
    return reinterpret_cast<int*>(this + 0x100);
}
