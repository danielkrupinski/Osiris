#include "Backtrack.h"
#include "Tickbase.h"

#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"

bool Tickbase::canShift(int ticks, bool shiftAnyways = false) noexcept
{
    if (!localPlayer || !localPlayer->isAlive() || !config->dt.enabled || ticks <= 0)
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
    if (!activeWeapon || !activeWeapon->clip() || activeWeapon->isInThrow())
        return false;

    if (activeWeapon->isKnife() || activeWeapon->isGrenade() || activeWeapon->isShotgun()
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
    if (!localPlayer || !localPlayer->isAlive() || !config->dt.enabled)
        return;
    if (!canShift(ticks, shiftAnyways))
        return;

   // tick->timesShifted++;
	
    tick->commandNumber = cmd->commandNumber;
    tick->tickbase = localPlayer->tickBase();
    tick->tickshift = ticks;
    //Teleport kinda buggy
    //tick->chokedPackets += ticks;
    //recalculateTicks();
}

void Tickbase::run(UserCmd* cmd) noexcept
{

    static void* oldNetwork = nullptr;
    if(auto network = interfaces->engine->getNetworkChannel(); network && oldNetwork != network)
    {
        oldNetwork = network;
        tick->ticksAllowedForProcessing = tick->maxUsercmdProcessticks;
        tick->chokedPackets = 0;
    }
    if (auto network = interfaces->engine->getNetworkChannel(); network && network->chokedPackets > tick->chokedPackets)
        tick->chokedPackets = network->chokedPackets;

    recalculateTicks();

    tick->ticks = cmd->tickCount;
    if (!localPlayer || !localPlayer->isAlive() || !config->dt.enabled)
        return;

    auto ticks = 0;

    switch (config->dt.mode) {
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

    if (config->dt.enabled && cmd->buttons & (UserCmd::IN_ATTACK))
    {
    //	tick->attempted++;
		shiftTicks(ticks, cmd);   
    }

    if (tick->tickshift <= 0 && tick->ticksAllowedForProcessing < (tick->maxUsercmdProcessticks - tick->fakeLag) && !config->misc.fakeDucking && ((config->misc.fakeLagTicks <= (tick->maxUsercmdProcessticks - ticks)) || !config->misc.fakeLagTicks))
    {
        cmd->tickCount = INT_MAX; //recharge
        tick->chokedPackets--;
    }

    recalculateTicks();
}