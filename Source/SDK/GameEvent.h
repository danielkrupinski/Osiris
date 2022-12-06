#pragma once

#include "UtlVector.h"
#include "VirtualMethod.h"

namespace csgo::pod { struct GameEvent; }

class GameEvent : public VirtualCallableFromPOD<GameEvent, csgo::pod::GameEvent> {
public:
    VIRTUAL_METHOD_V(const char*, getName, 1, (), ())
    VIRTUAL_METHOD_V(int, getInt, 6, (const char* keyName, int defaultValue = 0), (keyName, defaultValue))
    VIRTUAL_METHOD_V(float, getFloat, 8, (const char* keyName, float defaultValue = 0.0f), (keyName, defaultValue))
    VIRTUAL_METHOD_V(const char*, getString, 9, (const char* keyName, const char* defaultValue = ""), (keyName, defaultValue))
    VIRTUAL_METHOD_V(void, setInt, 13, (const char* keyName, int value), (keyName, value))
    VIRTUAL_METHOD_V(void, setString, 16, (const char* keyName, const char* value), (keyName, value))
};

class GameEventListener {
public:
    virtual ~GameEventListener() = default;
    virtual void fireGameEvent(csgo::pod::GameEvent* event) = 0;
    virtual int getEventDebugId() { return 42; }
};

class KeyValues;

class GameEventDescriptor {
public:
    int	eventid;
    int elementIndex;
    KeyValues* keys;
    UtlVector<void*> listeners;
};

namespace csgo::pod { struct GameEventManager; }
class GameEventManager : public VirtualCallableFromPOD<GameEventManager, csgo::pod::GameEventManager> {
public:
    GameEventManager(VirtualCallableFromPOD base, std::uintptr_t getEventDescriptorFn)
        : VirtualCallableFromPOD{ base }, getEventDescriptorFn{ getEventDescriptorFn }
    {
    }

    VIRTUAL_METHOD_V(bool, addListener, 3, (GameEventListener* listener, const char* name), (listener, name, false))
    VIRTUAL_METHOD_V(void, removeListener, 5, (GameEventListener* listener), (listener))

    [[nodiscard]] GameEventDescriptor* getEventDescriptor(const char* name, int* cookie) const noexcept
    {
        return getInvoker().invokeThiscall<GameEventDescriptor*>(getThis(), getEventDescriptorFn, name, cookie);
    }

private:
    std::uintptr_t getEventDescriptorFn;
};
