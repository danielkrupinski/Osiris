
#include "Ragebot.h"
#include "../Config.h"
#include "../interfaces.h"
#include "../Memory.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponId.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/PhysicsSurfaceProps.h"
#include "../SDK/WeaponData.h"
#include "../SDK/EngineTrace.h"

#define M_PI_F2		((float)(M_PI2))	// Shouldn't collide with anything.

#ifndef M_PI2
#define M_PI2		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#ifndef DEG2RAD2
#define DEG2RAD2( x  )  ( (float)(x) * (float)(M_PI_F2 / 180.f) )
#endif


void inline SinCos23(float radians, float* sine, float* cosine)
{
    *sine = sin(radians);
    *cosine = cos(radians);

}

void AngleVectors23(const Vector2& angles, Vector2* forward)
{
    float	sp, sy, cp, cy;

    SinCos23(DEG2RAD2(angles[1]), &sy, &cy);
    SinCos23(DEG2RAD2(angles[0]), &sp, &cp);

    forward->x = cp * cy;
    forward->y = cp * sy;
    forward->z = -sp;

}

void VectorAngles(const Vector2& forward, Vector2& angles)
{
    float tmp, yaw, pitch;

    if (forward[1] == 0 && forward[0] == 0)
    {
        yaw = 0;
        if (forward[2] > 0)
            pitch = 270;
        else
            pitch = 90;
    }
    else
    {
        yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
        if (yaw < 0)
            yaw += 360;

        tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
        pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
        if (pitch < 0)
            pitch += 360;
    }

    angles[0] = pitch;
    angles[1] = yaw;
    angles[2] = 0;
}



enum HitboxList
{
    HITBOX_HEAD,
    HITBOX_NECK,
    HITBOX_PELVIS,
    HITBOX_BODY,
    HITBOX_THORAX,
    HITBOX_CHEST,
    HITBOX_UPPER_CHEST,
    HITBOX_RIGHT_THIGH,
    HITBOX_LEFT_THIGH,
    HITBOX_RIGHT_CALF,
    HITBOX_LEFT_CALF,
    HITBOX_RIGHT_FOOT,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_HAND,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_UPPER_ARM,
    HITBOX_RIGHT_FOREARM,
    HITBOX_LEFT_UPPER_ARM,
    HITBOX_LEFT_FOREARM,
    HITBOX_MAX
};





static bool handleTaserPenetration(UserCmd* cmd, Vector& angle, Vector& target) noexcept
{
    Vector end;
    Trace enterTrace;
    __asm {
        mov ecx, end
        mov edx, enterTrace
    }

    interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), target }, 0x46004009, { localPlayer.get() }, enterTrace);

    if (sqrt(sqrt(enterTrace.startpos.x * enterTrace.startpos.y * enterTrace.startpos.z)) - sqrt(sqrt(target.x * target.y * target.z)) <= 26)
        return true;
    else
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



/*bool Ragebot::hitchance(VectorD angle, float chance, Entity* entity) {
    //auto chance = float(c_config::get()->i["rage_hitchance_value" + weapon_type]);

    //auto local_player = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());


    auto wep = localPlayer->getActiveWeapon();

    //	if (!wep)
        //	return false;

    if (wep->itemDefinitionIndex() == WEAPON_TASER)
        chance = 85.f;

    //	if ((local_player->eyeAngles2() - angle).Length() > wep->get_full_info()->m_Range)
        //	return false;

    VectorD
        forward,
        right,
        up,
        src = localPlayer->getEyePositionVectorD();

    Math_AngleVectors(angle, &forward, &right, &up);

    int
        hits = 0,
        needed_hits = static_cast<int>(500.f * (chance / 100.f));

    wep->UpdateAccuracyPenalty();

    float
        weap_spread = wep->GetSpread(),
        weap_inaccuracy = wep->getInaccuracy();

    for (int i = 0; i < 500; i++)
    {
        float
            a = Game_RandomFloat(0.f, 1.f),
            b = Game_RandomFloat(0.f, 2.f * M_PI),
            c = Game_RandomFloat(0.f, 1.f),
            d = Game_RandomFloat(0.f, 2.f * M_PI),
            inaccuracy = a * weap_inaccuracy,
            spread = c * weap_spread;

        if (wep->itemDefinitionIndex() == WEAPON_REVOLVER) {
            a = 1.f - a * a;
            c = 1.f - c * c;
        }

        VectorD
            spread_view((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0),
            direction;

        direction.x = forward.x + (spread_view.x * right.x) + (spread_view.y * up.x);
        direction.y = forward.y + (spread_view.x * right.y) + (spread_view.y * up.y);
        direction.z = forward.z + (spread_view.x * right.z) + (spread_view.y * up.z);
        direction.Normalized();

        VectorD
            viewangles_spread,
            view_forward;

        Math_VectorAngles(direction, up, viewangles_spread);
        Math_Normalize(viewangles_spread);

        Math_AngleVectors(viewangles_spread, view_forward);
        view_forward.NormalizeInPlace();

        view_forward = src + (view_forward * wep->get_full_info()->m_Range);

        trace_t tr;
        Ray_t ray;

        ray.Init(src, view_forward);
        memory.g_trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, entity, &tr);

        if (tr.m_pEnt == entity)
            hits++;

        if (static_cast<int>((static_cast<float>(hits) / 500.f) * 100.f) >= chance)
            return true;
    }

    return false;


}*/



Vector Ragebot::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    Vector delta = destination - source;
    Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))) - viewAngles.x,
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
    if (!memory->traceToExit(enterTrace.endpos.x, enterTrace.endpos.y, enterTrace.endpos.z, direction.x, direction.y, direction.z, exitTrace))
        return -1.0f;

    SurfaceData* exitSurfaceData = interfaces->physicsSurfaceProps->getSurfaceData(exitTrace.surface.surfaceProps);

    float damageModifier = 0.16f;
    float penetrationModifier = (enterSurfaceData->penetrationmodifier + exitSurfaceData->penetrationmodifier) / 2.0f;

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

static bool canScan(Entity* localPlayer, Entity* entity, const Vector& destination, const WeaponInfo* weaponData, int minDamage) noexcept
{
    float damage{ static_cast<float>(weaponData->damage) };

    Vector start{ localPlayer->getEyePosition() };
    Vector direction{ destination - start };
    direction /= direction.length();

    int hitsLeft = 4;

    while (damage >= 1.0f && hitsLeft) {
        static Trace trace;
        interfaces->engineTrace->traceRay({ start, destination }, 0x4600400B, localPlayer, trace);

        if (trace.fraction == 1.0f)
            break;

        if (trace.entity == entity && trace.hitgroup > HitGroup::Generic&& trace.hitgroup <= HitGroup::RightLeg) {
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

void Ragebot::run(UserCmd* cmd) noexcept
{
    if (!localPlayer || localPlayer->nextAttack() > memory->globalVars->serverTime())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->clip())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
    if (!weaponIndex)
        return;

    auto weaponClass = getWeaponClass(activeWeapon->itemDefinitionIndex2());
    if (!weaponClass || weaponClass == 63 || weaponClass == 40)
        return;

    if (!config->ragebot[weaponIndex].enabled)
        weaponIndex = 0;

    if (!config->ragebot[weaponIndex].betweenShots && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
        return;

    if (!config->ragebot[weaponIndex].ignoreFlash && localPlayer->flashDuration())
        return;


   // if (activeWeapon->itemDefinitionIndex2() == WeaponId::Taser && config->misc.autoZeus)
        //Ragebot::autoZeus(cmd);



    if (config->ragebot[weaponIndex].enabled && (cmd->buttons & UserCmd::IN_ATTACK || config->ragebot[weaponIndex].autoShot) && activeWeapon->getInaccuracy() <= config->ragebot[weaponIndex].maxAimInaccuracy) {

        if (config->ragebot[weaponIndex].scopedOnly && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
            return;

        auto bestFov = config->ragebot[weaponIndex].fov;
        Vector bestTarget{ };
        auto localPlayerEyePosition = localPlayer->getEyePosition();

        auto aimPunch = localPlayer->getAimPunch();
        aimPunch.x *= config->ragebot[weaponIndex].recoilControlY;
        aimPunch.y *= config->ragebot[weaponIndex].recoilControlX;

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            auto entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()
                || !entity->isEnemy() && !config->ragebot[weaponIndex].friendlyFire || entity->gunGameImmunity())
                continue;

            auto boneList = config->ragebot[weaponIndex].bone == 1 ? std::initializer_list{ 8, 4, 3, 7, 6, 5 } : std::initializer_list{ 8, 7, 6, 5, 4, 3 };

            for (auto bone : boneList) {
                auto bonePosition = entity->getBonePosition(config->ragebot[weaponIndex].bone > 1 ? 10 - config->ragebot[weaponIndex].bone : bone);
                if (!entity->isVisible(bonePosition) && (config->ragebot[weaponIndex].visibleOnly || !canScan(localPlayer.get(), entity, bonePosition, activeWeapon->getWeaponData(), config->ragebot[weaponIndex].killshot ? entity->health() : config->ragebot[weaponIndex].minDamage)))
                    continue;

                auto angle = calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + (config->ragebot[weaponIndex].recoilbasedFov ? aimPunch : Vector{ }));
                auto fov = std::hypotf(angle.x, angle.y);
                if (fov < bestFov) {
                    bestFov = fov;
                    bestTarget = bonePosition;
                }
                if (config->ragebot[weaponIndex].bone)
                    break;
            }
        }

        if (bestTarget && (config->ragebot[weaponIndex].ignoreSmoke || !memory->lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTarget, 1))) {
            static Vector lastAngles{ cmd->viewangles };
            static int lastCommand{ };

            if (lastCommand == cmd->commandNumber - 1 && lastAngles && config->ragebot[weaponIndex].silent)
                cmd->viewangles = lastAngles;

            auto angle = calculateRelativeAngle(localPlayer->getEyePosition(), bestTarget, cmd->viewangles + aimPunch);
            bool clamped{ false };

            if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
                angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
                angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
                clamped = true;
            }

            angle /= config->ragebot[weaponIndex].smooth;
            cmd->viewangles += angle;
            if (!config->ragebot[weaponIndex].silent)
                interfaces->engine->setViewAngles(cmd->viewangles);
            

            if (config->ragebot[weaponIndex].autoShot && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime() && !clamped && activeWeapon->getInaccuracy() <= config->ragebot[weaponIndex].maxShotInaccuracy)
                cmd->buttons |= UserCmd::IN_ATTACK;

            if (clamped)
                cmd->buttons &= ~UserCmd::IN_ATTACK;

            if (clamped || config->ragebot[weaponIndex].smooth > 1.0f) lastAngles = cmd->viewangles;
            else lastAngles = Vector{ };

            lastCommand = cmd->commandNumber;
        }
    }
}





bool Ragebot::can_shoot(const float time, UserCmd* cmd)
{
    
    if (!localPlayer) return false;
    if (localPlayer->health() <= 0) return false;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon || weapon->clip() == 0) return false;




    if (weapon->nextPrimaryAttack() > time)
        return false;

    if (localPlayer->nextAttack() > time)
        return false;
}


void Ragebot::Autostop(UserCmd* cmd) noexcept
{
    

    if (!localPlayer) return;

    Vector2 Velocity = localPlayer->velocity_vector2();

    if (Velocity.Length2D() == 0)
        return;

    static float Speed = 450.f;


    Vector2 Direction;
    Vector2 RealView;
    VectorAngles(Velocity, Direction);
    interfaces->engine->getViewAngles2(RealView);
    Direction.y = RealView.y - Direction.y;

    Vector2 Forward;
    AngleVectors23(Direction, &Forward);
    Vector2 NegativeDirection = Forward * -Speed;

    cmd->forwardmove = NegativeDirection.x;
    cmd->sidemove = NegativeDirection.y;
}