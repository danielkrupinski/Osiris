#pragma once

#include <SDK/GameEvent.h>

class EventListener : public GameEventListener {
public:
    explicit EventListener(const GameEventManager& gameEventManager);

    void fireGameEvent(csgo::pod::GameEvent* event) override;
    void remove();

private:
    const GameEventManager& gameEventManager;
};
