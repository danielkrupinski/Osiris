#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <memory>

#include "Aimbot.h"
#include "../Config.h"
#include "../InputUtil.h"
#include "../Memory.h"
#include "Misc.h"
#include <CSGO/Engine.h>
#include <CSGO/EngineTrace.h>
#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/UserCmd.h>
#include <CSGO/Vector.h>
#include <CSGO/WeaponId.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/PhysicsSurfaceProps.h>
#include <CSGO/WeaponData.h>

#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>

csgo::Vector Aimbot::calculateRelativeAngle(const csgo::Vector& source, const csgo::Vector& destination, const csgo::Vector& viewAngles) noexcept
{
    return ((destination - source).toAngle() - viewAngles).normalize();
}

static bool traceToExit(const Memory& memory, const csgo::Trace& enterTrace, const csgo::Vector& start, const csgo::Vector& direction, csgo::Vector& end, csgo::Trace& exitTrace)
{
    using csgo::Vector;

    bool result = false;
#if IS_WIN32() && (!defined(__clang__) || !defined(_DEBUG))
    const auto traceToExitFn = memory.traceToExit;
    __asm {
        push 0
        push 0
        push 0
        push exitTrace
        mov eax, direction
        push [eax]Vector.z
        push [eax]Vector.y
        push [eax]Vector.x
        mov eax, start
        push [eax]Vector.z
        push [eax]Vector.y
        push [eax]Vector.x
        mov edx, enterTrace
        mov ecx, end
        call traceToExitFn
        add esp, 40
        mov result, al
    }
#endif
    return result;
}

static float handleBulletPenetration(const OtherInterfaces& interfaces, const Memory& memory, csgo::SurfaceData* enterSurfaceData, const csgo::Trace& enterTrace, const csgo::Vector& direction, csgo::Vector& result, float penetration, float damage) noexcept
{
    csgo::Vector end;
    csgo::Trace exitTrace;

    if (!traceToExit(memory, enterTrace, enterTrace.endpos, direction, end, exitTrace))
        return -1.0f;

    csgo::SurfaceData* exitSurfaceData = interfaces.getPhysicsSurfaceProps().getSurfaceData(exitTrace.surface.surfaceProps);

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

static bool canScan(const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, const csgo::Entity& entity, const csgo::Vector& destination, const csgo::WeaponInfo* weaponData, int minDamage, bool allowFriendlyFire) noexcept
{
    if (!localPlayer)
        return false;

    float damage{ static_cast<float>(weaponData->damage) };

    csgo::Vector start{ localPlayer.get().getEyePosition() };
    csgo::Vector direction{ destination - start };
    direction /= direction.length();

    int hitsLeft = 4;

    while (damage >= 1.0f && hitsLeft) {
        csgo::Trace trace;
        engineInterfaces.engineTrace().traceRay({ start, destination }, 0x4600400B, localPlayer.get().getPOD(), trace);

        if (!allowFriendlyFire && trace.entity && csgo::Entity::from(retSpoofGadgets->client, trace.entity).isPlayer() && !localPlayer.get().isOtherEnemy(memory, csgo::Entity::from(retSpoofGadgets->client, trace.entity)))
            return false;

        if (trace.fraction == 1.0f)
            break;

        if (trace.entity == entity.getPOD() && trace.hitgroup > HitGroup::Generic && trace.hitgroup <= HitGroup::RightLeg) {
            damage = HitGroup::getDamageMultiplier(trace.hitgroup) * damage * std::pow(weaponData->rangeModifier, trace.fraction * weaponData->range / 500.0f);

            if (float armorRatio{ weaponData->armorRatio / 2.0f }; HitGroup::isArmored(trace.hitgroup, csgo::Entity::from(retSpoofGadgets->client, trace.entity).hasHelmet()))
                damage -= (csgo::Entity::from(retSpoofGadgets->client, trace.entity).armor() < damage * armorRatio / 2.0f ? csgo::Entity::from(retSpoofGadgets->client, trace.entity).armor() * 4.0f : damage) * (1.0f - armorRatio);

            return damage >= minDamage;
        }
        const auto surfaceData = interfaces.getPhysicsSurfaceProps().getSurfaceData(trace.surface.surfaceProps);

        if (surfaceData->penetrationmodifier < 0.1f)
            break;

        damage = handleBulletPenetration(interfaces, memory, surfaceData, trace, direction, start, weaponData->penetration, damage);
        hitsLeft--;
    }
    return false;
}

static bool keyPressed = false;

void Aimbot::updateInput(const Config& config) noexcept
{
    if (config.aimbotKeyMode == 0)
        keyPressed = config.aimbotKey.isDown();
    if (config.aimbotKeyMode == 1 && config.aimbotKey.isPressed())
        keyPressed = !keyPressed;
}

void Aimbot::run(Misc& misc, const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Config& config, const Memory& memory, csgo::UserCmd* cmd) noexcept
{
    if (!localPlayer || localPlayer.get().nextAttack() > memory.globalVars->serverTime() || localPlayer.get().isDefusing() || localPlayer.get().waitForNoAttack())
        return;

    const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getActiveWeapon());
    if (activeWeapon.getPOD() == nullptr || !activeWeapon.clip())
        return;

    if (localPlayer.get().shotsFired() > 0 && !activeWeapon.isFullAuto())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon.itemDefinitionIndex());
    if (!weaponIndex)
        return;

    auto weaponClass = getWeaponClass(activeWeapon.itemDefinitionIndex());
    if (!config.aimbot[weaponIndex].enabled)
        weaponIndex = weaponClass;

    if (!config.aimbot[weaponIndex].enabled)
        weaponIndex = 0;

    if (!config.aimbot[weaponIndex].betweenShots && activeWeapon.nextPrimaryAttack() > memory.globalVars->serverTime())
        return;

    if (!config.aimbot[weaponIndex].ignoreFlash && localPlayer.get().isFlashed())
        return;

    if (config.aimbotOnKey && !keyPressed)
        return;

    if (config.aimbot[weaponIndex].enabled && (cmd->buttons & csgo::UserCmd::IN_ATTACK || config.aimbot[weaponIndex].autoShot || config.aimbot[weaponIndex].aimlock) && activeWeapon.getInaccuracy() <= config.aimbot[weaponIndex].maxAimInaccuracy) {

        if (config.aimbot[weaponIndex].scopedOnly && activeWeapon.isSniperRifle() && !localPlayer.get().isScoped())
            return;

        auto bestFov = config.aimbot[weaponIndex].fov;
        csgo::Vector bestTarget{ };
        const auto localPlayerEyePosition = localPlayer.get().getEyePosition();

        const auto aimPunch = activeWeapon.requiresRecoilControl() ? localPlayer.get().getAimPunch() : csgo::Vector{ };

        for (int i = 1; i <= engineInterfaces.getEngine().getMaxClients(); i++) {
            const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(i));
            if (entity.getPOD() == nullptr || entity.getPOD() == localPlayer.get().getPOD() || entity.getNetworkable().isDormant() || !entity.isAlive()
                || !entity.isOtherEnemy(memory, localPlayer.get()) && !config.aimbot[weaponIndex].friendlyFire || entity.gunGameImmunity())
                continue;

            for (auto bone : { 8, 4, 3, 7, 6, 5 }) {
                const auto bonePosition = entity.getBonePosition(config.aimbot[weaponIndex].bone > 1 ? 10 - config.aimbot[weaponIndex].bone : bone);
                const auto angle = calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + aimPunch);
                
                const auto fov = std::hypot(angle.x, angle.y);
                if (fov > bestFov)
                    continue;

                if (!config.aimbot[weaponIndex].ignoreSmoke && memory.lineGoesThroughSmoke(localPlayerEyePosition, bonePosition, 1))
                    continue;

                if (!entity.isVisible(engineInterfaces.engineTrace(), bonePosition) && (config.aimbot[weaponIndex].visibleOnly || !canScan(engineInterfaces, interfaces, memory, entity, bonePosition, activeWeapon.getWeaponData(), config.aimbot[weaponIndex].killshot ? entity.health() : config.aimbot[weaponIndex].minDamage, config.aimbot[weaponIndex].friendlyFire)))
                    continue;

                if (fov < bestFov) {
                    bestFov = fov;
                    bestTarget = bonePosition;
                }
                if (config.aimbot[weaponIndex].bone)
                    break;
            }
        }

        if (bestTarget.notNull()) {
            static csgo::Vector lastAngles{ cmd->viewangles };
            static int lastCommand{ };

            if (lastCommand == cmd->commandNumber - 1 && lastAngles.notNull() && config.aimbot[weaponIndex].silent)
                cmd->viewangles = lastAngles;

            auto angle = calculateRelativeAngle(localPlayerEyePosition, bestTarget, cmd->viewangles + aimPunch);
            bool clamped{ false };

            if (std::abs(angle.x) > misc.maxAngleDelta() || std::abs(angle.y) > misc.maxAngleDelta()) {
                    angle.x = std::clamp(angle.x, -misc.maxAngleDelta(), misc.maxAngleDelta());
                    angle.y = std::clamp(angle.y, -misc.maxAngleDelta(), misc.maxAngleDelta());
                    clamped = true;
            }
            
            angle /= config.aimbot[weaponIndex].smooth;
            cmd->viewangles += angle;
            if (!config.aimbot[weaponIndex].silent)
                engineInterfaces.getEngine().setViewAngles(cmd->viewangles);

            if (config.aimbot[weaponIndex].autoScope && activeWeapon.nextPrimaryAttack() <= memory.globalVars->serverTime() && activeWeapon.isSniperRifle() && !localPlayer.get().isScoped())
                cmd->buttons |= csgo::UserCmd::IN_ATTACK2;

            if (config.aimbot[weaponIndex].autoShot && activeWeapon.nextPrimaryAttack() <= memory.globalVars->serverTime() && !clamped && activeWeapon.getInaccuracy() <= config.aimbot[weaponIndex].maxShotInaccuracy)
                cmd->buttons |= csgo::UserCmd::IN_ATTACK;

            if (clamped)
                cmd->buttons &= ~csgo::UserCmd::IN_ATTACK;

            if (clamped || config.aimbot[weaponIndex].smooth > 1.0f) lastAngles = cmd->viewangles;
            else lastAngles = csgo::Vector{ };

            lastCommand = cmd->commandNumber;
        }
    }
}
