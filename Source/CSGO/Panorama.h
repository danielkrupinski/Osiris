#pragma once

#include <Platform/Macros/CallingConventions.h>

#include "VirtualMethod.h"

namespace csgo
{

struct UIPanelPOD;

struct UIPanel : GameClass<UIPanel, UIPanelPOD> {
    VIRTUAL_METHOD_V(int, getChildCount, 48, (), ())
    VIRTUAL_METHOD_V(UIPanelPOD*, getChild, 49, (int n), (n))
    VIRTUAL_METHOD(bool, hasClass, 139, (const char* name), (name))
    VIRTUAL_METHOD(void, setHasClass, 145, (const char* name, bool hasClass), (name, hasClass))
    VIRTUAL_METHOD(float, getAttributeFloat, 278, (const char* name, float defaultValue), (name, defaultValue))
    VIRTUAL_METHOD(void, setAttributeFloat, WIN32_LINUX(288, 283), (const char* name, float value), (name, value))
};

struct PanoramaEventRegistration {
    int numberOfArgs;
    PAD(4);
    void* (CDECL_CONV* makeEvent)(void*);
    void* (CDECL_CONV* createEventFromString)(void*, const char* args, const char** result);
    PAD(WIN32_LINUX(24, 48));
};

struct UIEnginePOD;

struct UIEngine : GameClass<UIEngine, UIEnginePOD> {
    VIRTUAL_METHOD_V(void, dispatchEvent, 52, (void* eventPtr), (eventPtr))
};

struct PanoramaUIEnginePOD;

struct PanoramaUIEngine : GameClass<PanoramaUIEngine, PanoramaUIEnginePOD> {
    VIRTUAL_METHOD(UIEnginePOD*, accessUIEngine, 11, (), ())
};

}
