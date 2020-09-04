#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponId.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/PhysicsSurfaceProps.h"
#include "../SDK/WeaponData.h"
#include "EnginePrediction.h"

Vector Aimbot::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    return ((destination - source).toAngle() - viewAngles).normalize();
}

static float handleBulletPenetration(SurfaceData* enterSurfaceData, const Trace& enterTrace, const Vector& direction, Vector& result, float penetration, float damage) noexcept
{
    Vector end;
    Trace exitTrace;
    __asm {
        mov ecx, end
        mov edx, enterTrace
    }
    if (!memory->traceToExit(enterTrace.endpos.x, enterTrace.endpos.y, enterTrace.endpos.z, direction.x, direction.y, direction.z, exitTrace))
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

static bool handleTaserPenetration(UserCmd* cmd, Vector& angle, Vector& target) noexcept
{
    Vector end;
    Trace enterTrace;
    __asm {
        mov ecx, end
        mov edx, enterTrace
    }

    interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), target }, 0x46004009, { localPlayer.get() }, enterTrace);

    if (sqrt(sqrt(enterTrace.startpos.x * enterTrace.startpos.y * enterTrace.startpos.z)) - sqrt(sqrt(target.x * target.y * target.z)) <= config->misc.autoZeusMaxPenDist)
        return true;
    else
        return false;
}

void Aimbot::autoZeus(UserCmd* cmd) noexcept
{
    if (!localPlayer || !localPlayer->isAlive() || localPlayer->nextAttack() > memory->globalVars->serverTime())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->clip() || activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
    if (!weaponIndex)
        return;

    static auto lastTime = 0.0f;
    static auto lastContact = 0.0f;

    const auto now = memory->globalVars->realtime;

    const auto weaponData = activeWeapon->getWeaponData();
    if (!weaponData)
        return;

    if (activeWeapon->itemDefinitionIndex2() != WeaponId::Taser)
        return;

    Vector bestTarget{ };
    auto localPlayerEyePosition = localPlayer->getEyePosition();

    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get()) || entity->gunGameImmunity())
            continue;

        auto boneList = std::initializer_list{ 8, 4, 3, 7, 6, 5 };

        for (auto bone : boneList) {
            auto bonePosition = entity->getBonePosition(bone);

            auto angle = calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles);
            auto fov = std::hypotf(angle.x, angle.y);

            Vector viewAngles{ std::cos(degreesToRadians(cmd->viewangles.x + angle.x)) * std::cos(degreesToRadians(cmd->viewangles.y + angle.y)) * weaponData->range,
                std::cos(degreesToRadians(cmd->viewangles.x + angle.x)) * std::sin(degreesToRadians(cmd->viewangles.y + angle.y)) * weaponData->range,
                -std::sin(degreesToRadians(cmd->viewangles.x + angle.x)) * weaponData->range };

            if (!entity->isVisible(bonePosition) && !handleTaserPenetration(cmd, viewAngles, bonePosition))
                continue;
            else
            {
                Trace trace;

                interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, localPlayer.get(), trace);
                if (trace.entity && trace.entity->getClientClass()->classId == ClassId::CSPlayer && trace.entity->isOtherEnemy(localPlayer.get()) && !trace.entity->gunGameImmunity())
                {
                    float damage = (weaponData->damage * std::pow(weaponData->rangeModifier, trace.fraction * weaponData->range / 510.0f));

                    if (damage >= (true ? trace.entity->health() : 100)) {
                        bestTarget = bonePosition;
                    }
                }
            }
        }
    }

    if (bestTarget.notNull()) {
        static Vector lastAngles{ cmd->viewangles };
        static int lastCommand{ };

        if (lastCommand == cmd->commandNumber - 1 && lastAngles.notNull())
            cmd->viewangles = lastAngles;

        auto angle = calculateRelativeAngle(localPlayer->getEyePosition(), bestTarget, cmd->viewangles);
        bool clamped{ false };

        if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
            angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
            angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
            clamped = true;
        }

        cmd->viewangles += angle;

        const Vector viewAngles{ std::cos(degreesToRadians(cmd->viewangles.x)) * std::cos(degreesToRadians(cmd->viewangles.y)) * weaponData->range,
                 std::cos(degreesToRadians(cmd->viewangles.x)) * std::sin(degreesToRadians(cmd->viewangles.y)) * weaponData->range,
                -std::sin(degreesToRadians(cmd->viewangles.x)) * weaponData->range };
        Trace trace;
        interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, localPlayer.get(), trace);
        if (trace.entity && trace.entity->getClientClass()->classId == ClassId::CSPlayer && trace.entity->isOtherEnemy(localPlayer.get()) && !trace.entity->gunGameImmunity())
        {
            float damage = (weaponData->damage * std::pow(weaponData->rangeModifier, trace.fraction * weaponData->range / 500.0f));

            if (damage >= (true ? trace.entity->health() : 100)) {
                cmd->buttons |= UserCmd::IN_ATTACK;
                lastContact = now;
            }
        }

        if (clamped)
            cmd->buttons &= ~UserCmd::IN_ATTACK;

        if (clamped) lastAngles = cmd->viewangles;
        else lastAngles = Vector{ };

        lastCommand = cmd->commandNumber;
    }
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

static void setRandomSeed(int seed) noexcept
{
    using randomSeedFn = void(*)(int);
    static auto randomSeed{ reinterpret_cast<randomSeedFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed")) };
    randomSeed(seed);
}

static float getRandom(float min, float max) noexcept
{
    using randomFloatFn = float(*)(float, float);
    static auto randomFloat{ reinterpret_cast<randomFloatFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat")) };
    return randomFloat(min, max);
}

void Aimbot::handleKill(GameEvent& event) noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    lastKillTime = memory->globalVars->realtime;
    return;
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

    if (activeWeapon->itemDefinitionIndex2() == WeaponId::Taser && config->misc.autoZeus)
        Aimbot::autoZeus(cmd);

    if (!config->aimbot[weaponIndex].enabled)
        weaponIndex = weaponClass;

    if (!config->aimbot[weaponIndex].enabled)
        weaponIndex = 0;

    if ((EnginePrediction::getFlags() & 1) && !(localPlayer->flags() & 1) && config->aimbot[weaponIndex].jumpCheck)
        return;

    if (!config->aimbot[weaponIndex].betweenShots && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
        return;

    if (!config->aimbot[weaponIndex].ignoreFlash && localPlayer->isFlashed())
        return;

    if (lastKillTime + config->aimbot[weaponIndex].killDelay / 1000.0f > memory->globalVars->realtime)
        return;

    if (config->aimbot[weaponIndex].onKey) {
        if (!config->aimbot[weaponIndex].keyMode) {
            if (!GetAsyncKeyState(config->aimbot[weaponIndex].key))
                return;
        } else {
            static bool toggle = true;
            if (GetAsyncKeyState(config->aimbot[weaponIndex].key) & 1)
                toggle = !toggle;
            if (!toggle)
                return;
        }
    }

    if (config->aimbot[weaponIndex].enabled && (cmd->buttons & UserCmd::IN_ATTACK || config->aimbot[weaponIndex].autoShot || config->aimbot[weaponIndex].aimlock) && activeWeapon->getInaccuracy() <= config->aimbot[weaponIndex].maxAimInaccuracy) {

        if (config->aimbot[weaponIndex].scopedOnly && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
            return;

        auto bestFov = config->aimbot[weaponIndex].fov;
        Vector bestTarget{ };
        const auto localPlayerEyePosition = localPlayer->getEyePosition();

        auto aimPunch = activeWeapon->requiresRecoilControl() ? localPlayer->getAimPunch() : Vector{ };
        if (config->aimbot[weaponIndex].standaloneRCS && !config->aimbot[weaponIndex].silent) {
            static Vector lastAimPunch{ };
            if (localPlayer->getShotsFired() > config->aimbot[weaponIndex].shotsFired) {
                setRandomSeed(*memory->predictionRandomSeed);
                Vector currentPunch{ lastAimPunch.x - aimPunch.x, lastAimPunch.y - aimPunch.y, 0 };
                if (config->aimbot[weaponIndex].randomRCS) {
                    currentPunch.x *= getRandom(config->aimbot[weaponIndex].recoilControlX, 1.f);
                    currentPunch.y *= getRandom(config->aimbot[weaponIndex].recoilControlY, 1.f);
                }
                else {
                    currentPunch.x *= config->aimbot[weaponIndex].recoilControlX;
                    currentPunch.y *= config->aimbot[weaponIndex].recoilControlY;
                }
                cmd->viewangles += currentPunch;
            }
            interfaces->engine->setViewAngles(cmd->viewangles);
            lastAimPunch = aimPunch;
        }

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            auto entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()
                || !entity->isOtherEnemy(localPlayer.get()) && !config->aimbot[weaponIndex].friendlyFire || entity->gunGameImmunity())
                continue;

            auto boneList = config->aimbot[weaponIndex].bone == 1 ? std::initializer_list{ 8, 4, 3, 7, 6, 5 } : std::initializer_list{ 8, 7, 6, 5, 4, 3 };

            for (auto bone : boneList) {
                const auto bonePosition = entity->getBonePosition(config->aimbot[weaponIndex].bone > 1 ? 10 - config->aimbot[weaponIndex].bone : bone);

                if (!config->aimbot[weaponIndex].ignoreSmoke && memory->lineGoesThroughSmoke(localPlayerEyePosition, bonePosition, 1))
                    continue;

                if (!entity->isVisible(bonePosition) && (config->aimbot[weaponIndex].visibleOnly || !canScan(entity, bonePosition, activeWeapon->getWeaponData(), config->aimbot[weaponIndex].killshot ? entity->health() : config->aimbot[weaponIndex].minDamage, config->aimbot[weaponIndex].friendlyFire)))
                    continue;

                auto angle = calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + aimPunch);
                auto fov = std::hypotf(angle.x, angle.y);
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

            if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
                    angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
                    angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
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
