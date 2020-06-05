#pragma once

#include <memory>

#include "SDK/GameEvent.h"

class EventListener : public GameEventListener {
public:
    EventListener() noexcept;
    void remove() noexcept;
    void fireGameEvent(GameEvent* event);
};

inline std::unique_ptr<EventListener> eventListener;
