#include "Entity.h"

#include "../Memory.h"
#include "../Interfaces.h"
#include "GlobalVars.h"
#include "Localize.h"
#include "ModelInfo.h"
#include "../Hacks/Misc.h"

#include "Engine.h"
#include "EngineTrace.h"
#include "LocalPlayer.h"

bool Entity::setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
{
#ifdef _WIN32
    if (Misc::shouldFixBoneMatrix()) {
        int* render = reinterpret_cast<int*>(this + 0x274);
        int backup = *render;
        Vector absOrigin = getAbsOrigin();
        *render = 0;
        memory->setAbsOrigin(this, origin());
        auto result = VirtualMethod::call<bool, 13>(this + sizeof(uintptr_t), out, maxBones, boneMask, currentTime);
        memory->setAbsOrigin(this, absOrigin);
        *render = backup;
        return result;
    }
#endif
    return VirtualMethod::call<bool, 13>(this + sizeof(uintptr_t), out, maxBones, boneMask, currentTime);
}

Vector Entity::getBonePosition(int bone) noexcept
{
    if (matrix3x4 boneMatrices[256]; setupBones(boneMatrices, 256, 256, 0.0f))
        return boneMatrices[bone].origin();
    else
        return Vector{ };
}

bool Entity::isVisible(const Vector& position) noexcept
{
    if (!localPlayer)
        return false;

    Trace trace;
    interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), position.notNull() ? position : getBonePosition(8) }, 0x46004009, { localPlayer.get() }, trace);
    return trace.entity == this || trace.fraction > 0.97f;
}

bool Entity::isOtherEnemy(Entity* other) noexcept
{
    return memory->isOtherEnemy(this, other);
}

float Entity::getMaxDesyncAngle() noexcept
{
    const auto animState = getAnimstate();

    if (!animState)
        return 0.0f;

    float yawModifier = (animState->stopToFullRunningFraction * -0.3f - 0.2f) * std::clamp(animState->footSpeed, 0.0f, 1.0f) + 1.0f;

    if (animState->duckAmount > 0.0f)
        yawModifier += (animState->duckAmount * std::clamp(animState->footSpeed2, 0.0f, 1.0f) * (0.5f - yawModifier));

    return animState->velocitySubtractY * yawModifier;
}

int Entity::getUserId() noexcept
{
    if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(index(), playerInfo))
        return playerInfo.userId;
    return -1;
}

std::uint64_t Entity::getSteamId() noexcept
{
    if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(index(), playerInfo))
        return playerInfo.xuid;
    return 0;
}

[[nodiscard]] static wchar_t* removeNewlineChars(wchar_t* begin, wchar_t* end) noexcept
{
    return std::remove(begin, end, L'\n');
}

[[nodiscard]] static wchar_t* removeColorMarkup(wchar_t* begin, wchar_t* end) noexcept
{
    return std::remove_if(begin, end, [](wchar_t c) { return c > 0 && c < 17; });
}

[[nodiscard]] static wchar_t* removeConsecutiveSpaces(wchar_t* begin, wchar_t* end) noexcept
{
    return std::unique(begin, end, [](wchar_t a, wchar_t b) { return a == L' ' && a == b; });
}

void Entity::getPlayerName(char(&out)[128]) noexcept
{
    if (!*memory->playerResource) {
        strcpy(out, "unknown");
        return;
    }

    wchar_t wide[128];
    memory->getDecoratedPlayerName(*memory->playerResource, index(), wide, sizeof(wide), 4);

    auto end = removeNewlineChars(wide, wide + wcslen(wide));
    end = removeColorMarkup(wide, end);
    end = removeConsecutiveSpaces(wide, end);
    *end = L'\0';

    interfaces->localize->convertUnicodeToAnsi(wide, out, 128);
}

bool Entity::canSee(Entity* other, const Vector& pos) noexcept
{
    const auto eyePos = getEyePosition();
    if (memory->lineGoesThroughSmoke(eyePos, pos, 1))
        return false;

    Trace trace;
    interfaces->engineTrace->traceRay({ eyePos, pos }, 0x46004009, this, trace);
    return trace.entity == other || trace.fraction > 0.97f;
}

bool Entity::visibleTo(Entity* other) noexcept
{
    assert(isAlive());

    if (other->canSee(this, getAbsOrigin() + Vector{ 0.0f, 0.0f, 5.0f }))
        return true;

    if (other->canSee(this, getEyePosition() + Vector{ 0.0f, 0.0f, 5.0f }))
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

    for (const auto boxNum : { Hitbox::Belly, Hitbox::LeftForearm, Hitbox::RightForearm }) {
        const auto hitbox = set->getHitbox(boxNum);
        if (hitbox && other->canSee(this, boneMatrices[hitbox->bone].origin()))
            return true;
    }

    return false;
}
