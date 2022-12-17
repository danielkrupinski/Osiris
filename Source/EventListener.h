#pragma once

#include <CSGO/GameEvent.h>

class DefaultEventListener : public csgo::GameEventListener {
public:
    void fireGameEvent(csgo::GameEventPOD* eventPointer) override;
};

class EventListener : public csgo::GameEventListener {
public:
    explicit EventListener(csgo::GameEventManager gameEventManager);

    void fireGameEvent(csgo::GameEventPOD* event) override;
    void remove();

private:
    csgo::GameEventManager gameEventManager;
};
