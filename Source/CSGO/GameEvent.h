#pragma once

#include "Functions.h"
#include <RetSpoof/FunctionInvoker.h>
#include "UtlVector.h"
#include "VirtualMethod.h"

namespace csgo
{

struct GameEventPOD;
struct KeyValuesPOD;

struct GameEvent : GameClass<GameEvent, GameEventPOD> {
    VIRTUAL_METHOD_V(const char*, getName, 1, (), ())
    VIRTUAL_METHOD_V(int, getInt, 6, (const char* keyName, int defaultValue = 0), (keyName, defaultValue))
    VIRTUAL_METHOD_V(float, getFloat, 8, (const char* keyName, float defaultValue = 0.0f), (keyName, defaultValue))
    VIRTUAL_METHOD_V(const char*, getString, 9, (const char* keyName, const char* defaultValue = ""), (keyName, defaultValue))
    VIRTUAL_METHOD_V(void, setInt, 13, (const char* keyName, int value), (keyName, value))
    VIRTUAL_METHOD_V(void, setString, 16, (const char* keyName, const char* value), (keyName, value))
};

struct GameEventListener {
    virtual ~GameEventListener() = default;
    virtual void fireGameEvent(GameEventPOD* event) = 0;
    virtual int getEventDebugId() { return 42; }
};

struct GameEventDescriptor {
    int	eventid;
    int elementIndex;
    csgo::KeyValuesPOD* keys;
    UtlVector<void*> listeners;
};

struct GameEventManagerPOD;

struct GameEventManager : GameClass<GameEventManager, GameEventManagerPOD> {
    GameEventManager(GameClass base, csgo::GetEventDescriptor getEventDescriptorFn)
        : GameClass{ base }, getEventDescriptorFn{ getEventDescriptorFn }
    {
    }

    VIRTUAL_METHOD_V(bool, addListener, 3, (GameEventListener* listener, const char* name), (listener, name, false))
    VIRTUAL_METHOD_V(void, removeListener, 5, (GameEventListener* listener), (listener))

    [[nodiscard]] GameEventDescriptor* getEventDescriptor(const char* name, int* cookie) const noexcept
    {
        return FunctionInvoker{ getInvoker(), getEventDescriptorFn }(getPOD(), name, cookie);
    }

private:
    csgo::GetEventDescriptor getEventDescriptorFn;
};

}
