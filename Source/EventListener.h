#pragma once

#include <SDK/GameEvent.h>

class DefaultEventListener : public GameEventListener {
public:
    void fireGameEvent(csgo::pod::GameEvent* eventPointer) override;
};

class EventListener : public GameEventListener {
public:
    explicit EventListener(GameEventManager gameEventManager);

    void fireGameEvent(csgo::pod::GameEvent* event) override;
    void remove();

private:
    GameEventManager gameEventManager;
};
