#pragma once

#include <Platform/CallingConventions.h>

#include "Inconstructible.h"
#include "VirtualMethod.h"

using UIPanelPointer = std::uintptr_t;

class UIPanel : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2_V(int, getChildCount, 48, (), ())
    VIRTUAL_METHOD2_V(UIPanelPointer, getChild, 49, (int n), (n))
    VIRTUAL_METHOD2(bool, hasClass, 139, (const char* name), (name))
    VIRTUAL_METHOD2(void, setHasClass, 145, (const char* name, bool hasClass), (name, hasClass))
    VIRTUAL_METHOD2(float, getAttributeFloat, 278, (const char* name, float defaultValue), (name, defaultValue))
    VIRTUAL_METHOD2(void, setAttributeFloat, WIN32_LINUX(288, 283), (const char* name, float value), (name, value))
};

struct PanoramaEventRegistration {
    int numberOfArgs;
    PAD(4)
    void* (CDECL_CONV* makeEvent)(void*);
    void* (CDECL_CONV* createEventFromString)(void*, const char* args, const char** result);
    PAD(WIN32_LINUX(24, 48))
};

using UIEnginePointer = std::uintptr_t;

class UIEngine : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2_V(void, dispatchEvent, 52, (void* eventPtr), (eventPtr))
};

namespace csgo::pod { struct PanoramaUIEngine; }

class PanoramaUIEngine : public VirtualCallableFromPOD<PanoramaUIEngine, csgo::pod::PanoramaUIEngine> {
public:
    VIRTUAL_METHOD2(UIEnginePointer, accessUIEngine, 11, (), ())
};
