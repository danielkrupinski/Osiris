#include "Entity.h"

#include "../Memory.h"
#include "../Interfaces.h"
#include "ModelInfo.h"
#include "GlobalVars.h"

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

bool Entity::visibleTo(Entity* other) noexcept
{
    if (other->canSee(this, getAbsOrigin() + Vector{ 0.0f, 0.0f, 5.0f }))
        return true;

    const auto model = getModel();
    if (!model)
        return false;

    const auto studioModel = interfaces->modelInfo->getStudioModel(model);
    if (!studioModel)
        return false;

    const auto set = studioModel->getHitboxSet(hitboxSet());
    if (!set)
        return false;

    matrix3x4 boneMatrices[MAXSTUDIOBONES];
    if (!setupBones(boneMatrices, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, memory->globalVars->currenttime))
        return false;

    for (const auto boxNum : { 12, 9, 14, 17 }) { // head, guts, left & right elbow hitbox
        if (boxNum < set->numHitboxes && other->canSee(this, boneMatrices[set->getHitbox(boxNum)->bone].origin()))
            return true;
    }

    return false;
}
