#include "../Hacks/Backtrack.h"
#include "Tickbase.h"

#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"

bool canShift(int ticks, bool shiftAnyways = false)
{
    if (!localPlayer || !localPlayer->isAlive() || !config->ragebotExtra.enabled || ticks <= 0)
        return false;

    if (shiftAnyways)
        return true;

    if ((Tickbase::tick->ticksAllowedForProcessing - ticks) < 0)
        return false;

    if (localPlayer->nextAttack() > memory->globalVars->serverTime())
        return false;

    float nextAttack = (localPlayer->nextAttack() + (ticks * memory->globalVars->intervalPerTick));
    if (nextAttack >= memory->globalVars->serverTime())
        return false;

    auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->clip())// || activeWeapon->throwing()) commented this bc it needs UserCmd* and im too lazy 2 fix
        return false;

    if (activeWeapon->isKnife() || activeWeapon->isNade() || activeWeapon->isShotgun()
        || activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver
        || activeWeapon->itemDefinitionIndex2() == WeaponId::Awp
        || activeWeapon->itemDefinitionIndex2() == WeaponId::Ssg08
        || activeWeapon->itemDefinitionIndex2() == WeaponId::Taser
        || activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver)
        return false;

    float shiftTime = (localPlayer->tickBase() - ticks) * memory->globalVars->intervalPerTick;

    if (shiftTime < activeWeapon->nextPrimaryAttack())
        return false;

    return true;
}

void recalculateTicks() noexcept
{
    Tickbase::tick->chokedPackets = std::clamp(Tickbase::tick->chokedPackets, 0, Tickbase::tick->maxUsercmdProcessticks);
    Tickbase::tick->ticksAllowedForProcessing = Tickbase::tick->maxUsercmdProcessticks - Tickbase::tick->chokedPackets;
    Tickbase::tick->ticksAllowedForProcessing = std::clamp(Tickbase::tick->ticksAllowedForProcessing, 0, Tickbase::tick->maxUsercmdProcessticks);
}

void Tickbase::shiftTicks(int ticks, UserCmd* cmd, bool shiftAnyways) noexcept //useful, for other funcs
{
    if (!localPlayer || !localPlayer->isAlive() || !config->ragebotExtra.enabled)
        return;
    if (!canShift(ticks, shiftAnyways))
        return;

    tick->commandNumber = cmd->commandNumber;
    tick->tickbase = localPlayer->tickBase();
    tick->tickshift = ticks;
    Tickbase::lastShift = cmd->commandNumber;
    //Teleport kinda buggy
    //tick->chokedPackets += ticks;
    //recalculateTicks();
}

void Tickbase::run(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (config->ragebotExtra.doubleTapKey != 0) {
        if (config->ragebotExtra.doubleTapKeyMode == 0) {
            if (!GetAsyncKeyState(config->ragebotExtra.doubleTapKey))
            {
                config->ragebotExtra.doubleTapToggled = false;
            }
            else
                config->ragebotExtra.doubleTapToggled = true;
        }
        else {
            if (GetAsyncKeyState(config->ragebotExtra.doubleTapKey) & 1)
                config->ragebotExtra.doubleTapToggled = !config->ragebotExtra.doubleTapToggled;
        }
    }
    else
        config->ragebotExtra.doubleTapToggled = true;

    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick); };

    static void* oldNetwork = nullptr;
    if (auto network = interfaces->engine->getNetworkChannel(); network && oldNetwork != network)
    {
        oldNetwork = network;
        tick->ticksAllowedForProcessing = tick->maxUsercmdProcessticks;
        tick->chokedPackets = 0;
    }
    if (auto network = interfaces->engine->getNetworkChannel(); network && network->chokedPackets > tick->chokedPackets)
        tick->chokedPackets = network->chokedPackets;

    recalculateTicks();

    tick->ticks = cmd->tickCount;
    if (!localPlayer || !localPlayer->isAlive() || !config->ragebotExtra.enabled || (config->antiAim.general.fakeWalk.keyToggled && config->antiAim.general.fakeWalk.enabled))
        return;

    auto ticks = 0;

    switch (config->ragebotExtra.doubletapSpeed) {
    case 0: //Instant
        ticks = 16;
        break;
    case 1: //Fast
        ticks = 14;
        break;
    case 2: //Accurate
        ticks = 12;
        break;
    }

    if (config->ragebotExtra.doubletap && cmd->buttons & (UserCmd::IN_ATTACK) && config->ragebotExtra.doubleTapToggled)
        shiftTicks(ticks, cmd);

    if (tick->tickshift <= 0 && tick->ticksAllowedForProcessing < (tick->maxUsercmdProcessticks - tick->fakeLag) && !config->misc.fakeDucking && (cmd->commandNumber - lastShift) >= tick->maxUsercmdProcessticks && config->ragebotExtra.doubleTapToggled) //&& ((config->misc.fakeLagMode != 0 && config->misc.fakeLagTicks <= (tick->maxUsercmdProcessticks - ticks)) || !config->misc.fakeLagMode))
    {
        sendPacket = true;
        cmd->tickCount = INT_MAX; //recharge
        tick->chokedPackets--;
    }

    recalculateTicks();
}