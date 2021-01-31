#pragma once

#include "VirtualMethod.h"

class UIPanel {
public:
    VIRTUAL_METHOD_V(int, getChildCount, 48, (), (this))
    VIRTUAL_METHOD_V(UIPanel*, getChild, 49, (int n), (this, n))
    VIRTUAL_METHOD(bool, hasClass, 139, (const char* name), (this, name))
    VIRTUAL_METHOD(void, setHasClass, 145, (const char* name, bool hasClass), (this, name, hasClass))
    VIRTUAL_METHOD(float, getAttributeFloat, 278, (const char* name, float defaultValue), (this, name, defaultValue))
    VIRTUAL_METHOD(void, setAttributeFloat, WIN32_LINUX(288, 283), (const char* name, float value), (this, name, value))
};

struct PanoramaEventRegistration {
    PAD(8)
    void* (__CDECL* makeEvent)(void*);
    PAD(WIN32_LINUX(28, 56))
};

class UIEngine {
public:
    VIRTUAL_METHOD_V(void, dispatchEvent, 52, (void* eventPtr), (this, eventPtr))
};

class PanoramaUIEngine {
public:
    VIRTUAL_METHOD(UIEngine*, accessUIEngine, 11, (), (this))
};
