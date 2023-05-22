#include "Entity.h"

#include "../Memory.h"
#include "GlobalVars.h"
#include "Localize.h"
#include "ModelInfo.h"
#include "../Hacks/Misc.h"

#include "Engine.h"
#include "EngineTrace.h"
#include "LocalPlayer.h"

#include "matrix3x4.h"

#include <Interfaces/OtherInterfaces.h>

namespace csgo
{

bool Entity::setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) const noexcept
{
    return getRenderable().setupBones(out, maxBones, boneMask, currentTime);
}

Vector Entity::getBonePosition(int bone) const noexcept
{
    if (matrix3x4 boneMatrices[256]; setupBones(boneMatrices, 256, 256, 0.0f))
        return boneMatrices[bone].origin();
    else
        return Vector{ };
}

bool Entity::isVisible(const EngineTrace& engineTrace, const Vector& position) const noexcept
{
    if (!localPlayer)
        return false;

    Trace trace;
    engineTrace.traceRay({ localPlayer.get().getEyePosition(), position.notNull() ? position : getBonePosition(8) }, 0x46004009, { localPlayer.get().getPOD() }, trace);
    return trace.entity == getPOD() || trace.fraction > 0.97f;
}

bool Entity::isOtherEnemy(const Memory& memory, const Entity& other) const noexcept
{
    return memory.isOtherEnemy(getPOD(), other.getPOD());
}

float Entity::getMaxDesyncAngle() const noexcept
{
    const auto animState = getAnimstate();

    if (!animState)
        return 0.0f;

    float yawModifier = (animState->stopToFullRunningFraction * -0.3f - 0.2f) * std::clamp(animState->footSpeed, 0.0f, 1.0f) + 1.0f;

    if (animState->duckAmount > 0.0f)
        yawModifier += (animState->duckAmount * std::clamp(animState->footSpeed2, 0.0f, 1.0f) * (0.5f - yawModifier));

    return animState->velocitySubtractY * yawModifier;
}

int Entity::getUserId(const Engine& engine) const noexcept
{
    if (PlayerInfo playerInfo; engine.getPlayerInfo(getNetworkable().index(), playerInfo))
        return playerInfo.userId;
    return -1;
}

std::uint64_t Entity::getSteamId(const Engine& engine) const noexcept
{
    if (PlayerInfo playerInfo; engine.getPlayerInfo(getNetworkable().index(), playerInfo))
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

void Entity::getPlayerName(const OtherInterfaces& interfaces, const Memory& memory, char(&out)[128]) const noexcept
{
    if (!*memory.playerResource) {
        strcpy(out, "unknown");
        return;
    }

    wchar_t wide[128];
    memory.getDecoratedPlayerName(*memory.playerResource, getNetworkable().index(), wide, sizeof(wide), 4);

    auto end = removeNewlineChars(wide, wide + wcslen(wide));
    end = removeColorMarkup(wide, end);
    end = removeConsecutiveSpaces(wide, end);
    *end = L'\0';

    interfaces.getLocalize().convertUnicodeToAnsi(wide, out, 128);
}

bool Entity::canSee(const EngineTrace& engineTrace, const Memory& memory, const Entity& other, const Vector& pos) const noexcept
{
    const auto eyePos = getEyePosition();
    if (memory.lineGoesThroughSmoke(eyePos, pos, 1))
        return false;

    Trace trace;
    engineTrace.traceRay({ eyePos, pos }, 0x46004009, getPOD(), trace);
    return trace.entity == other.getPOD() || trace.fraction > 0.97f;
}

bool Entity::visibleTo(const EngineInterfaces& engineInterfaces, const Memory& memory, const Entity& other) const noexcept
{
    assert(isAlive());

    if (other.canSee(engineInterfaces.engineTrace(), memory, *this, getAbsOrigin() + Vector{ 0.0f, 0.0f, 5.0f }))
        return true;

    if (other.canSee(engineInterfaces.engineTrace(), memory, *this, getEyePosition() + Vector{ 0.0f, 0.0f, 5.0f }))
        return true;

    const auto model = getRenderable().getModel();
    if (!model)
        return false;

    const auto studioModel = engineInterfaces.getModelInfo().getStudioModel(model);
    if (!studioModel)
        return false;

    const auto set = studioModel->getHitboxSet(hitboxSet());
    if (!set)
        return false;

    matrix3x4 boneMatrices[MAXSTUDIOBONES];
    if (!setupBones(boneMatrices, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, memory.globalVars->currenttime))
        return false;

    for (const auto boxNum : { Hitbox::Belly, Hitbox::LeftForearm, Hitbox::RightForearm }) {
        const auto hitbox = set->getHitbox(boxNum);
        if (hitbox && other.canSee(engineInterfaces.engineTrace(), memory, *this, boneMatrices[hitbox->bone].origin()))
            return true;
    }

    return false;
}

}
