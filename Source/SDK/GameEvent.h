#pragma once

#include "UtlVector.h"
#include "VirtualMethod.h"

namespace csgo::pod { struct GameEvent; }

class GameEvent : public VirtualCallableFromPOD<GameEvent, csgo::pod::GameEvent> {
public:
    VIRTUAL_METHOD2_V(const char*, getName, 1, (), ())
    VIRTUAL_METHOD2_V(int, getInt, 6, (const char* keyName, int defaultValue = 0), (keyName, defaultValue))
    VIRTUAL_METHOD2_V(float, getFloat, 8, (const char* keyName, float defaultValue = 0.0f), (keyName, defaultValue))
    VIRTUAL_METHOD2_V(const char*, getString, 9, (const char* keyName, const char* defaultValue = ""), (keyName, defaultValue))
    VIRTUAL_METHOD2_V(void, setInt, 13, (const char* keyName, int value), (keyName, value))
    VIRTUAL_METHOD2_V(void, setString, 16, (const char* keyName, const char* value), (keyName, value))
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

class GameEventManager : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2_V(bool, addListener, 3, (GameEventListener* listener, const char* name), (listener, name, false))
    VIRTUAL_METHOD2_V(void, removeListener, 5, (GameEventListener* listener), (listener))
};
