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

namespace
{
    class EventListenerImpl : public GameEventListener {
    public:
        EventListenerImpl(const Memory& memory) : memory{ memory } {}

        void fireGameEvent(GameEvent* event) override
        {
            switch (fnv::hashRuntime(event->getName())) {
            case fnv::hash("round_start"):
                GameData::clearProjectileList();
                Misc::preserveKillfeed(memory, true);
                [[fallthrough]];
            case fnv::hash("round_freeze_end"):
                Misc::purchaseList(memory, event);
                break;
            case fnv::hash("player_death"): {
                auto& inventoryChanger = inventory_changer::InventoryChanger::instance(memory);
                inventoryChanger.updateStatTrak(*event);
                inventoryChanger.overrideHudIcon(memory, *event);
                Misc::killMessage(*event);
                Misc::killSound(*event);
                break;
            }
            case fnv::hash("player_hurt"):
                Misc::playHitSound(*event);
                Visuals::hitEffect(memory, event);
                Visuals::hitMarker(memory, event);
                break;
            case fnv::hash("vote_cast"):
                Misc::voteRevealer(memory, *event);
                break;
            case fnv::hash("round_mvp"):
                inventory_changer::InventoryChanger::instance(memory).onRoundMVP(*event);
                break;
            }
        }

        static EventListenerImpl& instance(const Memory& memory) noexcept
        {
            static EventListenerImpl impl{ memory };
            return impl;
        }

    private:
        const Memory& memory;
    };
}

void EventListener::init(const Memory& memory) noexcept
{
    assert(interfaces);

    // If you add here listeners which aren't used by client.dll (e.g., item_purchase, bullet_impact), the cheat will be detected by AntiDLL (community anticheat).
    // Instead, register listeners dynamically and only when certain functions are enabled - see Misc::updateEventListeners(), Visuals::updateEventListeners()

    const auto gameEventManager = interfaces->gameEventManager;
    gameEventManager->addListener(&EventListenerImpl::instance(memory), "round_start");
    gameEventManager->addListener(&EventListenerImpl::instance(memory), "round_freeze_end");
    gameEventManager->addListener(&EventListenerImpl::instance(memory), "player_hurt");
    gameEventManager->addListener(&EventListenerImpl::instance(memory), "player_death");
    gameEventManager->addListener(&EventListenerImpl::instance(memory), "vote_cast");
    gameEventManager->addListener(&EventListenerImpl::instance(memory), "round_mvp");

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

void EventListener::remove(const Memory& memory) noexcept
{
    assert(interfaces);

    interfaces->gameEventManager->removeListener(&EventListenerImpl::instance(memory));
}
