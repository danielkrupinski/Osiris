#include "Entity.h"

#include "../Memory.h"

bool Entity::isOtherEnemy(Entity* other) noexcept
{
    return memory->isOtherEnemy(this, other);
}

bool Entity::canSee(Entity* other, const Vector& pos) noexcept
{
    Trace trace;
    interfaces->engineTrace->traceRay({ getEyePosition(), pos }, 0x46004009, this, trace);
    return (trace.entity == other || trace.fraction > 0.97f) && !memory->lineGoesThroughSmoke(getEyePosition(), pos, 1);
}
