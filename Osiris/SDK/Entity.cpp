#include "Entity.h"

#include "../Memory.h"

bool Entity::isOtherEnemy(Entity* other) noexcept
{
    return memory->isOtherEnemy(this, other);
}
