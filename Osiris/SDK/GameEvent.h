#pragma once

#include "VirtualMethod.h"

class GameEvent {
public:
    VIRTUAL_METHOD(const char*, getName, 1, (), (this))
    VIRTUAL_METHOD(int, getInt, 6, (const char* keyName, int defaultValue = 0), (this, keyName, defaultValue))
    VIRTUAL_METHOD(float, getFloat, 8, (const char* keyName, float defaultValue = 0.0f), (this, keyName, defaultValue))
    VIRTUAL_METHOD(const char*, getString, 9, (const char* keyName, const char* defaultValue = ""), (this, keyName, defaultValue))
    VIRTUAL_METHOD(void, setString, 16, (const char* keyName, const char* value), (this, keyName, value))
};

class GameEventListener {
public:
    virtual ~GameEventListener() {}
    virtual void fireGameEvent(GameEvent* event) = 0;
    virtual int getEventDebugId() { return 42; }
};

class GameEventManager {
public:
    VIRTUAL_METHOD(bool, addListener, 3, (GameEventListener* listener, const char* name), (this, listener, name, false))
    VIRTUAL_METHOD(void, removeListener, 5, (GameEventListener* listener), (this, listener))
};
