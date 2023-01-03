#include <cassert>
#include <utility>

#include "EventListener.h"
#include "fnv.h"
#include "GameData.h"
#include "Hacks/Misc.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Hacks/Visuals.h"
#include "Memory.h"
#include "CSGO/Constants/GameEventNames.h"
#include "CSGO/GameEvent.h"
#include "CSGO/UtlVector.h"

#include "GlobalContext.h"

EventListener::EventListener(csgo::GameEventManager gameEventManager)
    : gameEventManager{ gameEventManager }
{
    // If you add here listeners which aren't used by client.dll (e.g., item_purchase, bullet_impact), the cheat will be detected by AntiDLL (community anticheat).
    // Instead, register listeners dynamically and only when certain functions are enabled - see Misc::updateEventListeners(), Visuals::updateEventListeners()

    gameEventManager.addListener(this, csgo::round_start);
    gameEventManager.addListener(this, csgo::round_freeze_end);
    gameEventManager.addListener(this, csgo::player_hurt);
    gameEventManager.addListener(this, csgo::player_death);
    gameEventManager.addListener(this, csgo::vote_cast);
    gameEventManager.addListener(this, csgo::round_mvp);

    // Move our player_death listener to the first position to override killfeed icons (InventoryChanger::overrideHudIcon()) before HUD gets them
    if (const auto desc = gameEventManager.getEventDescriptor(csgo::player_death, nullptr))
        std::swap(desc->listeners[0], desc->listeners[desc->listeners.size - 1]);
    else
        assert(false);

    // Move our round_mvp listener to the first position to override event data (InventoryChanger::onRoundMVP()) before HUD gets them
    if (const auto desc = gameEventManager.getEventDescriptor(csgo::round_mvp, nullptr))
        std::swap(desc->listeners[0], desc->listeners[desc->listeners.size - 1]);
    else
        assert(false);
}

void EventListener::remove()
{
    gameEventManager.removeListener(this);
}
