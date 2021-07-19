#include <algorithm>
#include <array>
#include <initializer_list>
#include <memory>

#include "Aimbot.h"
#include "../Config.h"
#include "../InputUtil.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "Misc.h"
#include "../SDK/Engine.h"
#include "../SDK/EngineTrace.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponId.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/PhysicsSurfaceProps.h"
#include "../SDK/WeaponData.h"
#include "../SDK/ClassId.h"
#include "../SDK/ClientClass.h"
#include "../SDK/BSPFlags.h"


Vector Aimbot::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    return ((destination - source).toAngle() - viewAngles).normalize();
}

bool IsBreakableEntity(Entity* entityPtr)
{
    if (!entityPtr || !entityPtr->index())
        return false;

    const auto result = memory->isBreakableFn(entityPtr);

    if (!result) {
        return false;
    }

    const ClientClass* cclass = entityPtr->getClientClass();

    if (!cclass) {
        return false;
    }

    const ClassId currentClassId = cclass->classId;

    if (!result &&
        (currentClassId == ClassId::CBaseDoor ||
            currentClassId == ClassId::CBreakableSurface ||
            (currentClassId == ClassId::CBaseEntity && entityPtr->getCollideable()->get_solid() == 1)))
        return true;

    return result;
}

bool traceToExit(Trace* enter_trace, Vector start, Vector dir, Trace* exit_trace)
{
#if defined(_WIN32)
    Vector end = { 0.f,0.f,0.f };
    float distance = 0.f;
    signed int distCheck = 23;
    int firstContents = 0;

    do
    {
        distance += 4.f;
        end = start + dir * distance;

        if (!firstContents) firstContents = interfaces->engineTrace->getPointContents(end, mask_shot | contents_grate);

        const int pointContents = interfaces->engineTrace->getPointContents(end, mask_shot | contents_grate);

        if (!(pointContents & (mask_shot_hull | contents_hitbox)) || pointContents & contents_hitbox && pointContents != firstContents)
        {
            const Vector newEnd = end - (dir * 4.f);

            const Ray ray(end, newEnd);

            interfaces->engineTrace->traceRay(ray, mask_shot | contents_grate, nullptr, *exit_trace);

            if (exit_trace->startSolid && exit_trace->surface.flags & surf_hitbox)
            {
                interfaces->engineTrace->traceRay({ end, start }, mask_shot_hull | contents_hitbox, exit_trace->entity, *exit_trace);

                if (exit_trace->DidHit() && !exit_trace->startSolid) return true;

                continue;
            }

            if (exit_trace->DidHit() && !exit_trace->startSolid)
            {
                if (enter_trace->surface.flags & surf_nodraw || !(exit_trace->surface.flags & surf_nodraw)) {
                    if (exit_trace->plane.normal.dotProduct(dir) <= 1.f)
                        return true;

                    continue;
                }

                if (IsBreakableEntity(enter_trace->entity)
                    && IsBreakableEntity(exit_trace->entity))
                    return true;

                continue;
            }

            if (exit_trace->surface.flags & surf_nodraw)
            {
                if (IsBreakableEntity(enter_trace->entity)
                    && IsBreakableEntity(exit_trace->entity))
                {
                    return true;
                }
                else if (!(enter_trace->surface.flags & surf_nodraw))
                {
                    continue;
                }
            }

            if ((!enter_trace->entity
                || enter_trace->entity->index() == 0)
                && (IsBreakableEntity(enter_trace->entity)))
            {
                exit_trace = enter_trace;
                exit_trace->endpos = start + dir;
                return true;
            }
            continue;
        }
        distCheck--;
    } while (distCheck);

    return false;
#endif
    //linux users, no tracetoexit for you :(
    return false;
}

static float handleBulletPenetration(SurfaceData* enterSurfaceData, Trace& enterTrace, const Vector& direction, Vector& result, float penetration, float damage) noexcept
{
    Vector end;
    Trace exitTrace;

    if (!traceToExit(&enterTrace, enterTrace.endpos, direction, &exitTrace))
        return -1.0f;

    SurfaceData* exitSurfaceData = interfaces->physicsSurfaceProps->getSurfaceData(exitTrace.surface.surfaceProps);

    float damageModifier = 0.16f;
    float penetrationModifier = (enterSurfaceData->penetrationmodifier + exitSurfaceData->penetrationmodifier) / 2.0f;

    if (enterSurfaceData->material == 71 || enterSurfaceData->material == 89) {
        damageModifier = 0.05f;
        penetrationModifier = 3.0f;
    } else if (enterTrace.contents >> 3 & 1 || enterTrace.surface.flags >> 7 & 1) {
        penetrationModifier = 1.0f;
    }

    if (enterSurfaceData->material == exitSurfaceData->material) {
        if (exitSurfaceData->material == 85 || exitSurfaceData->material == 87)
            penetrationModifier = 3.0f;
        else if (exitSurfaceData->material == 76)
            penetrationModifier = 2.0f;
    }

    damage -= 11.25f / penetration / penetrationModifier + damage * damageModifier + (exitTrace.endpos - enterTrace.endpos).squareLength() / 24.0f / penetrationModifier;

    result = exitTrace.endpos;
    return damage;
}

static bool canScan(Entity* entity, const Vector& destination, const WeaponInfo* weaponData, int minDamage, bool allowFriendlyFire) noexcept
{
    if (!localPlayer)
        return false;

    float damage{ static_cast<float>(weaponData->damage) };

    Vector start{ localPlayer->getEyePosition() };
    Vector direction{ destination - start };
    direction /= direction.length();

    int hitsLeft = 4;

    while (damage >= 1.0f && hitsLeft) {
        Trace trace;
        interfaces->engineTrace->traceRay({ start, destination }, 0x4600400B, localPlayer.get(), trace);

        if (!allowFriendlyFire && trace.entity && trace.entity->isPlayer() && !localPlayer->isOtherEnemy(trace.entity))
            return false;

        if (trace.fraction == 1.0f)
            break;

        if (trace.entity == entity && trace.hitgroup > HitGroup::Generic && trace.hitgroup <= HitGroup::RightLeg) {
            damage = HitGroup::getDamageMultiplier(trace.hitgroup) * damage * powf(weaponData->rangeModifier, trace.fraction * weaponData->range / 500.0f);

            if (float armorRatio{ weaponData->armorRatio / 2.0f }; HitGroup::isArmored(trace.hitgroup, trace.entity->hasHelmet()))
                damage -= (trace.entity->armor() < damage * armorRatio / 2.0f ? trace.entity->armor() * 4.0f : damage) * (1.0f - armorRatio);

            return damage >= minDamage;
        }
        const auto surfaceData = interfaces->physicsSurfaceProps->getSurfaceData(trace.surface.surfaceProps);

        if (surfaceData->penetrationmodifier < 0.1f)
            break;

        damage = handleBulletPenetration(surfaceData, trace, direction, start, weaponData->penetration, damage);
        hitsLeft--;
    }
    return false;
}

static bool keyPressed = false;

void Aimbot::updateInput() noexcept
{
    if (config->aimbotKeyMode == 0)
        keyPressed = config->aimbotKey.isDown();
    if (config->aimbotKeyMode == 1 && config->aimbotKey.isPressed())
        keyPressed = !keyPressed;
}

void Aimbot::run(UserCmd* cmd) noexcept
{
    if (!localPlayer || localPlayer->nextAttack() > memory->globalVars->serverTime() || localPlayer->isDefusing() || localPlayer->waitForNoAttack())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->clip())
        return;

    if (localPlayer->shotsFired() > 0 && !activeWeapon->isFullAuto())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
    if (!weaponIndex)
        return;

    auto weaponClass = getWeaponClass(activeWeapon->itemDefinitionIndex2());
    if (!config->aimbot[weaponIndex].enabled)
        weaponIndex = weaponClass;

    if (!config->aimbot[weaponIndex].enabled)
        weaponIndex = 0;

    if (!config->aimbot[weaponIndex].betweenShots && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
        return;

    if (!config->aimbot[weaponIndex].ignoreFlash && localPlayer->isFlashed())
        return;

    if (config->aimbotOnKey && !keyPressed)
        return;

    if (config->aimbot[weaponIndex].enabled && (cmd->buttons & UserCmd::IN_ATTACK || config->aimbot[weaponIndex].autoShot || config->aimbot[weaponIndex].aimlock) && activeWeapon->getInaccuracy() <= config->aimbot[weaponIndex].maxAimInaccuracy) {

        if (config->aimbot[weaponIndex].scopedOnly && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
            return;

        auto bestFov = config->aimbot[weaponIndex].fov;
        Vector bestTarget{ };
        const auto localPlayerEyePosition = localPlayer->getEyePosition();

        const auto aimPunch = activeWeapon->requiresRecoilControl() ? localPlayer->getAimPunch() : Vector{ };

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            auto entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()
                || !entity->isOtherEnemy(localPlayer.get()) && !config->aimbot[weaponIndex].friendlyFire || entity->gunGameImmunity())
                continue;

            for (auto bone : { 8, 4, 3, 7, 6, 5 }) {
                const auto bonePosition = entity->getBonePosition(config->aimbot[weaponIndex].bone > 1 ? 10 - config->aimbot[weaponIndex].bone : bone);
                const auto angle = calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + aimPunch);
                
                const auto fov = std::hypot(angle.x, angle.y);
                if (fov > bestFov)
                    continue;

                if (!config->aimbot[weaponIndex].ignoreSmoke && memory->lineGoesThroughSmoke(localPlayerEyePosition, bonePosition, 1))
                    continue;

                if (!entity->isVisible(bonePosition) && (config->aimbot[weaponIndex].visibleOnly || !canScan(entity, bonePosition, activeWeapon->getWeaponData(), config->aimbot[weaponIndex].killshot ? entity->health() : config->aimbot[weaponIndex].minDamage, config->aimbot[weaponIndex].friendlyFire)))
                    continue;

                if (fov < bestFov) {
                    bestFov = fov;
                    bestTarget = bonePosition;
                }
                if (config->aimbot[weaponIndex].bone)
                    break;
            }
        }

        if (bestTarget.notNull()) {
            static Vector lastAngles{ cmd->viewangles };
            static int lastCommand{ };

            if (lastCommand == cmd->commandNumber - 1 && lastAngles.notNull() && config->aimbot[weaponIndex].silent)
                cmd->viewangles = lastAngles;

            auto angle = calculateRelativeAngle(localPlayerEyePosition, bestTarget, cmd->viewangles + aimPunch);
            bool clamped{ false };

            if (std::abs(angle.x) > Misc::maxAngleDelta() || std::abs(angle.y) > Misc::maxAngleDelta()) {
                    angle.x = std::clamp(angle.x, -Misc::maxAngleDelta(), Misc::maxAngleDelta());
                    angle.y = std::clamp(angle.y, -Misc::maxAngleDelta(), Misc::maxAngleDelta());
                    clamped = true;
            }
            
            angle /= config->aimbot[weaponIndex].smooth;
            cmd->viewangles += angle;
            if (!config->aimbot[weaponIndex].silent)
                interfaces->engine->setViewAngles(cmd->viewangles);

            if (config->aimbot[weaponIndex].autoScope && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime() && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
                cmd->buttons |= UserCmd::IN_ATTACK2;

            if (config->aimbot[weaponIndex].autoShot && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime() && !clamped && activeWeapon->getInaccuracy() <= config->aimbot[weaponIndex].maxShotInaccuracy)
                cmd->buttons |= UserCmd::IN_ATTACK;

            if (clamped)
                cmd->buttons &= ~UserCmd::IN_ATTACK;

            if (clamped || config->aimbot[weaponIndex].smooth > 1.0f) lastAngles = cmd->viewangles;
            else lastAngles = Vector{ };

            lastCommand = cmd->commandNumber;
        }
    }
}
