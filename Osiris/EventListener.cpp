#include <cassert>

#include "EventListener.h"
#include "fnv.h"
#include "Hacks/Misc.h"
#include "Interfaces.h"

EventListener::EventListener() noexcept
{
    assert(interfaces);

    interfaces->gameEventManager->addListener(this, "item_purchase");
    interfaces->gameEventManager->addListener(this, "round_start");
    interfaces->gameEventManager->addListener(this, "round_freeze_end");
}

EventListener::~EventListener()
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
    }
}
