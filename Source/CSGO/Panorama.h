#pragma once

#include <Platform/Macros/CallingConventions.h>
#include <Utils/TypeHint.h>

#include "VirtualMethod.h"

namespace csgo
{

struct UIPanelPOD;

class UIPanel : public VirtualCallableFromPOD<UIPanel, UIPanelPOD> {
public:
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
    TypeHint<std::uintptr_t, void* (CDECL_CONV*)(void*)> makeEvent;
    TypeHint<std::uintptr_t, void* (CDECL_CONV*)(void*, const char* args, const char** result)> createEventFromString;
    PAD(WIN32_LINUX(24, 48));
};

struct UIEnginePOD;

class UIEngine : public VirtualCallableFromPOD<UIEngine, UIEnginePOD> {
public:
    VIRTUAL_METHOD_V(void, dispatchEvent, 52, (void* eventPtr), (eventPtr))
};

struct PanoramaUIEnginePOD;

class PanoramaUIEngine : public VirtualCallableFromPOD<PanoramaUIEngine, PanoramaUIEnginePOD> {
public:
    VIRTUAL_METHOD(UIEnginePOD*, accessUIEngine, 11, (), ())
};

}
