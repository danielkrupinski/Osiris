#pragma once

#include "Inconstructible.h"
#include "UtlVector.h"
#include "VirtualMethod.h"

class GameEvent {
public:
    INCONSTRUCTIBLE(GameEvent)

    VIRTUAL_METHOD_V(const char*, getName, 1, (), (this))
    VIRTUAL_METHOD_V(int, getInt, 6, (const char* keyName, int defaultValue = 0), (this, keyName, defaultValue))
    VIRTUAL_METHOD_V(float, getFloat, 8, (const char* keyName, float defaultValue = 0.0f), (this, keyName, defaultValue))
    VIRTUAL_METHOD_V(const char*, getString, 9, (const char* keyName, const char* defaultValue = ""), (this, keyName, defaultValue))
    VIRTUAL_METHOD_V(void, setInt, 13, (const char* keyName, int value), (this, keyName, value))
    VIRTUAL_METHOD_V(void, setString, 16, (const char* keyName, const char* value), (this, keyName, value))
};

class GameEventListener {
public:
    virtual ~GameEventListener() {}
    virtual void fireGameEvent(GameEvent* event) = 0;
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

class GameEventManager {
public:
    INCONSTRUCTIBLE(GameEventManager)

    VIRTUAL_METHOD_V(bool, addListener, 3, (GameEventListener* listener, const char* name), (this, listener, name, false))
    VIRTUAL_METHOD_V(void, removeListener, 5, (GameEventListener* listener), (this, listener))
};
