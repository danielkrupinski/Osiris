#include <random>
#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponId.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/PhysicsSurfaceProps.h"
#include "../SDK/WeaponData.h"
#include "../SDK/Angle.h"

Vector Aimbot::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    Vector delta{ destination - source };
    Vector angles{ radiansToDegrees(atan2f(-delta.z, delta.length2D())) - viewAngles.x,
                   radiansToDegrees(atan2f(delta.y, delta.x)) - viewAngles.y };
    angles.normalize();
    return angles;
}

static float handleBulletPenetration(SurfaceData* enterSurfaceData, const Trace& enterTrace, const Vector& direction, Vector& result, float penetration, float damage) noexcept
{
    Vector end;
    Trace exitTrace;
    __asm {
        mov ecx, end
        mov edx, enterTrace
    }
    if (!memory.traceToExit(enterTrace.endpos.x, enterTrace.endpos.y, enterTrace.endpos.z, direction.x, direction.y, direction.z, exitTrace))
        return -1.0f;

    SurfaceData* exitSurfaceData{ interfaces.physicsSurfaceProps->getSurfaceData(exitTrace.surface.surfaceProps) };

    float damageModifier{ 0.16f };
    float penetrationModifier{ (enterSurfaceData->penetrationmodifier + exitSurfaceData->penetrationmodifier) / 2.0f };

    if (enterSurfaceData->material == 71 || enterSurfaceData->material == 89) {
        damageModifier = 0.05f;
        penetrationModifier = 3.0f;
    }
    else if (enterTrace.contents >> 3 & 1 || enterTrace.surface.flags >> 7 & 1) {
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

static bool canScan(Entity* localPlayer, Entity* entity, const Vector& destination, const WeaponData* weaponData, int minDamage) noexcept
{
    float damage{ static_cast<float>(weaponData->damage) };

    Vector start{ localPlayer->getEyePosition() };
    Vector direction{ destination - start };
    direction /= direction.length();

    int hitsLeft{ 4 };

    while (damage >= 1.0f && hitsLeft) {
        static Trace trace;
        interfaces.engineTrace->traceRay({ start, destination }, 0x4600400B, localPlayer, trace);

        if (trace.fraction == 1.0f)
            break;

        if (trace.entity == entity && trace.hitgroup > HitGroup::Generic&& trace.hitgroup <= HitGroup::RightLeg) {
            damage = HitGroup::getDamageMultiplier(trace.hitgroup) * damage * powf(weaponData->rangeModifier, trace.fraction * weaponData->range / 500.0f);

            if (float armorRatio{ weaponData->armorRatio / 2.0f }; HitGroup::isArmored(trace.hitgroup, trace.entity->hasHelmet()))
                damage -= (trace.entity->armor() < damage * armorRatio / 2.0f ? trace.entity->armor() * 4.0f : damage) * (1.0f - armorRatio);

            return damage >= minDamage;
        }
        const auto surfaceData{ interfaces.physicsSurfaceProps->getSurfaceData(trace.surface.surfaceProps) };

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

static bool hitChance(Entity* localPlayer, Entity* entity, Entity* weaponData, const Vector& destinantion, const UserCmd* cmd, const int hitChance) noexcept
{
    if (!hitChance)
        return true;

    constexpr int maxSeed{ 256 };

    const Angle angles(destinantion + cmd->viewangles);

    int hits{ 0 };
    const int hitsNeed{ static_cast<int>(static_cast<float>(maxSeed)* (static_cast<float>(hitChance) / 100.f)) };

    const auto weapSpread{ weaponData->getSpread() };
    const auto weapInaccuracy{ weaponData->getInaccuracy() };
    const auto localEyePosition{ localPlayer->getEyePosition() };
    const auto range{ weaponData->getWeaponData()->range };

    for (int i = 0; i < maxSeed; ++i) {
        setRandomSeed(i + 1);
        float inaccuracy{ getRandom(0.f, 1.f) };
        float spread{ getRandom(0.f, 1.f) };
        const float spreadX{ getRandom(0.f, 2.f * static_cast<float>(M_PI)) };
        const float spreadY{ getRandom(0.f, 2.f * static_cast<float>(M_PI)) };

        const auto weaponIndex{ weaponData->itemDefinitionIndex2() };
        const auto recoilIndex{ weaponData->recoilIndex() };
        if (weaponIndex == WeaponId::Revolver)
        {
            if (cmd->buttons & UserCmd::IN_ATTACK2)
            {
                inaccuracy = 1.f - inaccuracy * inaccuracy;
                spread = 1.f - spread * spread;
            }
        }
        else if (weaponIndex == WeaponId::Negev && recoilIndex < 3.f)
        {
            for (int i = 3; i > recoilIndex; --i)
            {
                inaccuracy *= inaccuracy;
                spread *= spread;
            }

            inaccuracy = 1.f - inaccuracy;
            spread = 1.f - spread;
        }

        inaccuracy *= weapInaccuracy;
        spread *= weapSpread;

        Vector spreadView{ (cosf(spreadX) * inaccuracy) + (cosf(spreadY) * spread),
                           (sinf(spreadX) * inaccuracy) + (sinf(spreadY) * spread) };
        Vector direction{ (angles.forward + (angles.right * spreadView.x) + (angles.up * spreadView.y)) * range };

        static Trace trace;
        interfaces.engineTrace->clipRayToEntity({ localEyePosition, localEyePosition + direction }, 0x4600400B, entity, trace);
        if (trace.entity == entity)
            ++hits;

        if (hits >= hitsNeed)
            return true;

        if ((maxSeed - i + hits) < hitsNeed)
            return false;
    }
    return false;
}

void Aimbot::run(UserCmd* cmd) noexcept
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    if (localPlayer->nextAttack() > memory.globalVars->serverTime())
        return;

    const auto activeWeapon{ localPlayer->getActiveWeapon() };
    if (!activeWeapon || !activeWeapon->clip())
        return;

    auto weaponIndex{ getWeaponIndex(activeWeapon->itemDefinitionIndex2()) };
    if (!weaponIndex)
        return;

    const auto weaponClass{ getWeaponClass(activeWeapon->itemDefinitionIndex2()) };
    if (!config.aimbot[weaponIndex].enabled)
        weaponIndex = weaponClass;

    if (!config.aimbot[weaponIndex].enabled)
        weaponIndex = 0;

    if (config.aimbot[weaponIndex].enabled && config.aimbot[weaponIndex].standaloneRCS && config.aimbot[weaponIndex].rcsStyle == 1) {

        static Vector StaticAimPunchAngle;

        if (cmd->buttons & UserCmd::IN_ATTACK && localPlayer->getShotsFired() > config.aimbot[weaponIndex].shotsFired)
        {
            static auto SRCSweaponRecoilScale = interfaces.cvar->findVar("weapon_recoil_scale");
            auto CurrentAimPunchAngle = localPlayer->aimPunchAngle() * SRCSweaponRecoilScale->getFloat();
            CurrentAimPunchAngle.x *= config.aimbot[weaponIndex].recoilControlY;
            CurrentAimPunchAngle.y *= config.aimbot[weaponIndex].recoilControlX;
            cmd->viewangles += (StaticAimPunchAngle - CurrentAimPunchAngle);
            StaticAimPunchAngle = CurrentAimPunchAngle;
        }
        else
        {
            static auto SRCSweaponRecoilScale = interfaces.cvar->findVar("weapon_recoil_scale");
            auto AfterCurrentAimPunchAngle = localPlayer->aimPunchAngle() * SRCSweaponRecoilScale->getFloat();
            AfterCurrentAimPunchAngle.x *= config.aimbot[weaponIndex].recoilControlY;
            AfterCurrentAimPunchAngle.y *= config.aimbot[weaponIndex].recoilControlX;
            StaticAimPunchAngle = AfterCurrentAimPunchAngle;
        }

        interfaces.engine->setViewAngles(cmd->viewangles);

    }

    if (!config.aimbot[weaponIndex].betweenShots && activeWeapon->nextPrimaryAttack() > memory.globalVars->serverTime())
        return;

    if (!config.aimbot[weaponIndex].ignoreFlash && localPlayer->flashDuration())
        return;

    if (config.aimbot[weaponIndex].onKey) {
        if (!config.aimbot[weaponIndex].keyMode) {
            if (!GetAsyncKeyState(config.aimbot[weaponIndex].key))
                return;
        }
        else {
            static bool toggle{ true };
            if (GetAsyncKeyState(config.aimbot[weaponIndex].key) & 1)
                toggle = !toggle;
            if (!toggle)
                return;
        }
    }

    if (config.aimbot[weaponIndex].enabled && (cmd->buttons & UserCmd::IN_ATTACK || config.aimbot[weaponIndex].autoShot || config.aimbot[weaponIndex].aimlock)) {

        if (config.aimbot[weaponIndex].scopedOnly && activeWeapon->isSniperRifle() && !localPlayer->isScoped()) {
            if (config.aimbot[weaponIndex].autoScope)
                cmd->buttons |= UserCmd::IN_ATTACK2;
            return;
        }

        auto bestFov{ config.aimbot[weaponIndex].fov };

        std::vector<Enemies> enemies;
        for (int i = 1; i <= interfaces.engine->getMaxClients(); ++i) {
            const auto entity{ interfaces.entityList->getEntity(i) };
            if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive()
                || !entity->isEnemy() && !config.aimbot[weaponIndex].friendlyFire || entity->gunGameImmunity())
                continue;

            const auto origin{ entity->getAbsOrigin() };
            const auto localPlayerOrigin{ localPlayer->getAbsOrigin() };
            const auto health{ entity->health() };
            const auto distance{ localPlayerOrigin.distance(origin) };
            enemies.emplace_back(i, health, distance);
        }

        std::sort(enemies.begin(), enemies.end());

        const auto localPlayerEyePosition{ localPlayer->getEyePosition() };

        constexpr auto velocityExtrapolate{
            [](Entity* entity, const Vector& destination) constexpr noexcept {
                return destination + (entity->velocity() * memory.globalVars->intervalPerTick);
            }
        };

        const static auto weaponRecoilScale{ interfaces.cvar->findVar("weapon_recoil_scale") };
        auto aimPunch{ localPlayer->aimPunchAngle() * weaponRecoilScale->getFloat() };
        if (config.aimbot[weaponIndex].standaloneRCS && config.aimbot[weaponIndex].rcsStyle == 0 && !config.aimbot[weaponIndex].silent) {
            static Vector lastAimPunch{ };
            if (localPlayer->shotsFired() > 1) {
                setRandomSeed(*memory.predictionRandomSeed);
                Vector currentPunch{ lastAimPunch.x - aimPunch.x, lastAimPunch.y - aimPunch.y, 0 };
                currentPunch.x *= getRandom(config.aimbot[weaponIndex].recoilControlY, 1.f);
                currentPunch.y *= getRandom(config.aimbot[weaponIndex].recoilControlX, 1.f);
                cmd->viewangles += currentPunch;
            }
            interfaces.engine->setViewAngles(cmd->viewangles);
            lastAimPunch = aimPunch;
        }
        else {
            aimPunch.x *= config.aimbot[weaponIndex].recoilControlY;
            aimPunch.y *= config.aimbot[weaponIndex].recoilControlX;
        }

        Vector bestTarget{ };
        Vector bestAngle{ };
        const auto boneList{ config.aimbot[weaponIndex].bone == 1 ? std::initializer_list{ 8, 4, 3, 7, 6, 5 } : std::initializer_list{ 8, 7, 6, 5, 4, 3 } };
        for (const auto& target : enemies) {
            const auto entity{ interfaces.entityList->getEntity(target.id) };
            for (auto bone : boneList) {
                const auto bonePosition{ entity->getBonePosition(config.aimbot[weaponIndex].bone > 1 ? 10 - config.aimbot[weaponIndex].bone : bone) };
                const auto angle{ calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + (config.aimbot[weaponIndex].recoilbasedFov ? aimPunch : Vector{ })) };
                const auto fov{ angle.length2D() };
                if (!entity->isVisible(bonePosition) && (config.aimbot[weaponIndex].visibleOnly || !canScan(localPlayer, entity, bonePosition, activeWeapon->getWeaponData(), config.aimbot[weaponIndex].killshot ? target.health : (std::min)(config.aimbot[weaponIndex].minDamage, target.health))))
                    continue;

                if (fov < bestFov) {
                    bestFov = fov;
                    bestTarget = config.aimbot[weaponIndex].velocityExtrapolation ? velocityExtrapolate(entity, bonePosition) : bonePosition;
                    bestAngle = angle;
                }

                if (config.aimbot[weaponIndex].bone)
                    break;
            }

            if (bestTarget) {
                if (!hitChance(localPlayer, entity, activeWeapon, bestAngle, cmd, config.aimbot[weaponIndex].hitChance)) {
                    bestTarget = Vector{ };
                    continue;
                }
                break;
            }
        }

        if (bestTarget && (config.aimbot[weaponIndex].ignoreSmoke
            || !memory.lineGoesThroughSmoke(localPlayerEyePosition, bestTarget, 1))) {
            static Vector lastAngles{ cmd->viewangles };
            static int lastCommand{ };

            if (lastCommand == cmd->commandNumber - 1 && lastAngles && config.aimbot[weaponIndex].silent)
                cmd->viewangles = lastAngles;

            auto angle{ calculateRelativeAngle(localPlayerEyePosition, bestTarget, cmd->viewangles + aimPunch) };
            bool clamped{ false };

            if (fabs(angle.x) > config.misc.maxAngleDelta || fabs(angle.y) > config.misc.maxAngleDelta) {
                angle.x = std::clamp(angle.x, -config.misc.maxAngleDelta, config.misc.maxAngleDelta);
                angle.y = std::clamp(angle.y, -config.misc.maxAngleDelta, config.misc.maxAngleDelta);
                clamped = true;
            }

            angle /= config.aimbot[weaponIndex].smooth;
            cmd->viewangles += angle;
            if (!config.aimbot[weaponIndex].silent)
                interfaces.engine->setViewAngles(cmd->viewangles);

            if (config.aimbot[weaponIndex].autoShot && activeWeapon->nextPrimaryAttack() <= memory.globalVars->serverTime() && !clamped)
                cmd->buttons |= UserCmd::IN_ATTACK;

            if (clamped)
                cmd->buttons &= ~UserCmd::IN_ATTACK;

            if (clamped || config.aimbot[weaponIndex].smooth > 1.0f) lastAngles = cmd->viewangles;
            else lastAngles = Vector{ };

            lastCommand = cmd->commandNumber;
        }
    }
}