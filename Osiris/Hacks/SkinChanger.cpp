#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "SkinChanger.h"
#include "../Config.h"

extern void post_data_update_start(Entity*);

static bool hudUpdateRequired{ false };

static constexpr void updateHud() noexcept
{
    if (auto hud_weapons = memory.findHudElement(memory.hud, "CCSGO_HudWeaponSelection") - 0x28) {
        for (int i = 0; i < *(hud_weapons + 0x20); i++)
            i = memory.clearHudWeapon(hud_weapons, i);
    }
    hudUpdateRequired = false;
}

void SkinChanger::run(FrameStage stage) noexcept
{
    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        if (const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()); localPlayer && localPlayer->isAlive()) {
            post_data_update_start(localPlayer);
            if (hudUpdateRequired && !localPlayer->isDormant())
                updateHud();
        }
    }
}

void SkinChanger::scheduleHudUpdate() noexcept
{
    hudUpdateRequired = true;
}
