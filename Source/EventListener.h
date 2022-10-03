#pragma once

#include "Interfaces.h"
#include "Memory.h"

#include <SDK/GameEvent.h>

class EventListener : public GameEventListener {
public:
    EventListener(const Memory& memory, const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const Interfaces& interfaces);

    void fireGameEvent(GameEvent* event) override;
    void remove();

private:
    const Memory& memory;
    const ClientInterfaces& clientInterfaces;
    const EngineInterfaces& engineInterfaces;
    const Interfaces& interfaces;
};
