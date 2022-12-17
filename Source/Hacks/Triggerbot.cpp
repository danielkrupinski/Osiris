#include "../Config.h"
#include "../Memory.h"
#include <CSGO/EngineTrace.h>
#include <CSGO/Entity.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/UserCmd.h>
#include <CSGO/WeaponData.h>
#include <CSGO/WeaponId.h>
#include "Triggerbot.h"

static bool keyPressed;

void Triggerbot::run(const csgo::EngineTrace& engineTrace, const OtherInterfaces& interfaces, const Memory& memory, const Config& config, csgo::UserCmd* cmd) noexcept
{
    if (!localPlayer || !localPlayer.get().isAlive() || localPlayer.get().nextAttack() > memory.globalVars->serverTime() || localPlayer.get().isDefusing() || localPlayer.get().waitForNoAttack())
        return;

    const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getActiveWeapon());
    if (activeWeapon.getPOD() == nullptr || !activeWeapon.clip() || activeWeapon.nextPrimaryAttack() > memory.globalVars->serverTime())
        return;

    if (localPlayer.get().shotsFired() > 0 && !activeWeapon.isFullAuto())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon.itemDefinitionIndex());
    if (!weaponIndex)
        return;

    if (!config.triggerbot[weaponIndex].enabled)
        weaponIndex = getWeaponClass(activeWeapon.itemDefinitionIndex());

    if (!config.triggerbot[weaponIndex].enabled)
        weaponIndex = 0;

    const auto& cfg = config.triggerbot[weaponIndex];

    if (!cfg.enabled)
        return;

    static auto lastTime = 0.0f;
    static auto lastContact = 0.0f;

    const auto now = memory.globalVars->realtime;

    if (now - lastContact < config.triggerbot[weaponIndex].burstTime) {
        cmd->buttons |= csgo::UserCmd::IN_ATTACK;
        return;
    }
    lastContact = 0.0f;

    if (!keyPressed)
        return;

    if (now - lastTime < cfg.shotDelay / 1000.0f)
        return;

    if (!cfg.ignoreFlash && localPlayer.get().isFlashed())
        return;

    if (cfg.scopedOnly && activeWeapon.isSniperRifle() && !localPlayer.get().isScoped())
        return;

    const auto weaponData = activeWeapon.getWeaponData();
    if (!weaponData)
        return;

    const auto startPos = localPlayer.get().getEyePosition();
    const auto endPos = startPos + csgo::Vector::fromAngle(cmd->viewangles + localPlayer.get().getAimPunch()) * weaponData->range;

    if (!cfg.ignoreSmoke && memory.lineGoesThroughSmoke(startPos, endPos, 1))
        return;

    csgo::Trace trace;
    engineTrace.traceRay({ startPos, endPos }, 0x46004009, localPlayer.get().getPOD(), trace);

    lastTime = now;

    const auto entity = csgo::Entity::from(retSpoofGadgets->client, trace.entity);
    if (entity.getPOD() == nullptr || !entity.isPlayer())
        return;

    if (!cfg.friendlyFire && !localPlayer.get().isOtherEnemy(memory, entity))
        return;

    if (entity.gunGameImmunity())
        return;

    if (cfg.hitgroup && trace.hitgroup != cfg.hitgroup)
        return;

    float damage = (activeWeapon.itemDefinitionIndex() != WeaponId::Taser ? HitGroup::getDamageMultiplier(trace.hitgroup) : 1.0f) * weaponData->damage * std::pow(weaponData->rangeModifier, trace.fraction * weaponData->range / 500.0f);

    if (float armorRatio{ weaponData->armorRatio / 2.0f }; activeWeapon.itemDefinitionIndex() != WeaponId::Taser && HitGroup::isArmored(trace.hitgroup, entity.hasHelmet()))
        damage -= (entity.armor() < damage * armorRatio / 2.0f ? entity.armor() * 4.0f : damage) * (1.0f - armorRatio);

    if (damage >= (cfg.killshot ? entity.health() : cfg.minDamage)) {
        cmd->buttons |= csgo::UserCmd::IN_ATTACK;
        lastTime = 0.0f;
        lastContact = now;
    }
}

void Triggerbot::updateInput(const Config& config) noexcept
{
    keyPressed = !config.triggerbotHoldKey.isSet() || config.triggerbotHoldKey.isDown();
}
