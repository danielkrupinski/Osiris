#pragma once

#include <SDK/GameEvent.h>

class DefaultEventListener : public csgo::GameEventListener {
public:
    void fireGameEvent(csgo::pod::GameEvent* eventPointer) override;
};

class EventListener : public csgo::GameEventListener {
public:
    explicit EventListener(csgo::GameEventManager gameEventManager);

    void fireGameEvent(csgo::pod::GameEvent* event) override;
    void remove();

private:
    csgo::GameEventManager gameEventManager;
};
