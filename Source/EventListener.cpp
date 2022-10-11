#include <cassert>
#include <utility>

#include "EventListener.h"
#include "fnv.h"
#include "GameData.h"
#include "Hacks/Misc.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Hacks/Visuals.h"
#include "Interfaces.h"
#include "Memory.h"
#include "SDK/GameEvent.h"
#include "SDK/UtlVector.h"

#include "GlobalContext.h"

EventListener::EventListener(const Memory& memory, const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const Interfaces& interfaces)
    : memory{ memory }, clientInterfaces{ clientInterfaces }, engineInterfaces{ engineInterfaces }, interfaces{ interfaces }
{
    // If you add here listeners which aren't used by client.dll (e.g., item_purchase, bullet_impact), the cheat will be detected by AntiDLL (community anticheat).
    // Instead, register listeners dynamically and only when certain functions are enabled - see Misc::updateEventListeners(), Visuals::updateEventListeners()

    const auto gameEventManager = engineInterfaces.getGameEventManager();
    gameEventManager.addListener(this, "round_start");
    gameEventManager.addListener(this, "round_freeze_end");
    gameEventManager.addListener(this, "player_hurt");
    gameEventManager.addListener(this, "player_death");
    gameEventManager.addListener(this, "vote_cast");
    gameEventManager.addListener(this, "round_mvp");

    // Move our player_death listener to the first position to override killfeed icons (InventoryChanger::overrideHudIcon()) before HUD gets them
    if (const auto desc = memory.getEventDescriptor(engineInterfaces.getGameEventManagerAddress(), "player_death", nullptr))
        std::swap(desc->listeners[0], desc->listeners[desc->listeners.size - 1]);
    else
        assert(false);

    // Move our round_mvp listener to the first position to override event data (InventoryChanger::onRoundMVP()) before HUD gets them
    if (const auto desc = memory.getEventDescriptor(engineInterfaces.getGameEventManagerAddress(), "round_mvp", nullptr))
        std::swap(desc->listeners[0], desc->listeners[desc->listeners.size - 1]);
    else
        assert(false);
}

void EventListener::fireGameEvent(GameEvent* event)
{
    globalContext->fireGameEventCallback(event);
}

void EventListener::remove()
{
    engineInterfaces.getGameEventManager().removeListener(this);
}
