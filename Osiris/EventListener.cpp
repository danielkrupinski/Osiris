#include <cassert>

#include "EventListener.h"
#include "fnv.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Visuals.h"
#include "Interfaces.h"

EventListener::EventListener() noexcept
{
    assert(interfaces);

    interfaces->gameEventManager->addListener(this, "item_purchase");
    interfaces->gameEventManager->addListener(this, "round_start");
    interfaces->gameEventManager->addListener(this, "round_freeze_end");
    interfaces->gameEventManager->addListener(this, "player_death");
    interfaces->gameEventManager->addListener(this, "player_hurt");
}

void EventListener::remove() noexcept
{
    assert(interfaces);

    interfaces->gameEventManager->removeListener(this);
}

void EventListener::fireGameEvent(GameEvent* event)
{
    switch (fnv::hashRuntime(event->getName())) {
    case fnv::hash("round_start"):

    case fnv::hash("item_purchase"):
    case fnv::hash("round_freeze_end"):
        Misc::purchaseList(event);
        break;
    case fnv::hash("player_death"):
        SkinChanger::updateStatTrak(*event);
        Misc::killMessage(*event);
        Misc::killSound(*event);
        break;
    case fnv::hash("player_hurt"):
        Misc::playHitSound(*event);
        Visuals::hitEffect(event);
        Visuals::hitMarker(event);
        break;
    }
}
