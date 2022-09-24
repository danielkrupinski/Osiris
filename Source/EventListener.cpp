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

EventListener::EventListener(const Memory& memory, const Interfaces& interfaces)
    : memory{ memory }, interfaces{ interfaces }
{
    // If you add here listeners which aren't used by client.dll (e.g., item_purchase, bullet_impact), the cheat will be detected by AntiDLL (community anticheat).
    // Instead, register listeners dynamically and only when certain functions are enabled - see Misc::updateEventListeners(), Visuals::updateEventListeners()

    const auto gameEventManager = interfaces.gameEventManager;
    gameEventManager->addListener(this, "round_start");
    gameEventManager->addListener(this, "round_freeze_end");
    gameEventManager->addListener(this, "player_hurt");
    gameEventManager->addListener(this, "player_death");
    gameEventManager->addListener(this, "vote_cast");
    gameEventManager->addListener(this, "round_mvp");

    // Move our player_death listener to the first position to override killfeed icons (InventoryChanger::overrideHudIcon()) before HUD gets them
    if (const auto desc = memory.getEventDescriptor(gameEventManager, "player_death", nullptr))
        std::swap(desc->listeners[0], desc->listeners[desc->listeners.size - 1]);
    else
        assert(false);

    // Move our round_mvp listener to the first position to override event data (InventoryChanger::onRoundMVP()) before HUD gets them
    if (const auto desc = memory.getEventDescriptor(gameEventManager, "round_mvp", nullptr))
        std::swap(desc->listeners[0], desc->listeners[desc->listeners.size - 1]);
    else
        assert(false);
}

void EventListener::fireGameEvent(GameEvent* event)
{
    switch (fnv::hashRuntime(event->getName())) {
    case fnv::hash("round_start"):
        GameData::clearProjectileList();
        Misc::preserveKillfeed(memory, true);
        [[fallthrough]];
    case fnv::hash("round_freeze_end"):
        Misc::purchaseList(interfaces, memory, event);
        break;
    case fnv::hash("player_death"): {
        auto& inventoryChanger = inventory_changer::InventoryChanger::instance(interfaces, memory);
        inventoryChanger.updateStatTrak(interfaces, *event);
        inventoryChanger.overrideHudIcon(interfaces, memory, *event);
        Misc::killMessage(interfaces, *event);
        Misc::killSound(interfaces, *event);
        break;
    }
    case fnv::hash("player_hurt"):
        Misc::playHitSound(interfaces, *event);
        Visuals::hitEffect(interfaces, memory, event);
        Visuals::hitMarker(interfaces, memory, event);
        break;
    case fnv::hash("vote_cast"):
        Misc::voteRevealer(interfaces, memory, *event);
        break;
    case fnv::hash("round_mvp"):
        inventory_changer::InventoryChanger::instance(interfaces, memory).onRoundMVP(interfaces, *event);
        break;
    }
}

void EventListener::remove()
{
    interfaces.gameEventManager->removeListener(this);
}
