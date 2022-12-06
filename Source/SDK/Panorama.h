#pragma once

#include <Platform/CallingConventions.h>
#include <Utils/TypeHint.h>

#include "Inconstructible.h"
#include "VirtualMethod.h"

namespace csgo::pod { struct UIPanel; }

class UIPanel : public VirtualCallableFromPOD<UIPanel, csgo::pod::UIPanel> {
public:
    VIRTUAL_METHOD_V(int, getChildCount, 48, (), ())
    VIRTUAL_METHOD_V(csgo::pod::UIPanel*, getChild, 49, (int n), (n))
    VIRTUAL_METHOD(bool, hasClass, 139, (const char* name), (name))
    VIRTUAL_METHOD(void, setHasClass, 145, (const char* name, bool hasClass), (name, hasClass))
    VIRTUAL_METHOD(float, getAttributeFloat, 278, (const char* name, float defaultValue), (name, defaultValue))
    VIRTUAL_METHOD(void, setAttributeFloat, WIN32_LINUX(288, 283), (const char* name, float value), (name, value))
};

struct PanoramaEventRegistration {
    int numberOfArgs;
    PAD(4)
    TypeHint<std::uintptr_t, void*(CDECL_CONV*)(void*)> makeEvent;
    TypeHint<std::uintptr_t, void*(CDECL_CONV*)(void*, const char* args, const char** result)> createEventFromString;
    PAD(WIN32_LINUX(24, 48))
};

using UIEnginePointer = std::uintptr_t;

class UIEngine : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD_V(void, dispatchEvent, 52, (void* eventPtr), (eventPtr))
};

namespace csgo::pod { struct PanoramaUIEngine; }

class PanoramaUIEngine : public VirtualCallableFromPOD<PanoramaUIEngine, csgo::pod::PanoramaUIEngine> {
public:
    VIRTUAL_METHOD(UIEnginePointer, accessUIEngine, 11, (), ())
};
