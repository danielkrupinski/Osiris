#pragma once

#include <Platform/Macros/PlatformSpecific.h>
#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct PanoramaMarshallHelper; }

class PanoramaMarshallHelper : public VirtualCallableFromPOD<PanoramaMarshallHelper, pod::PanoramaMarshallHelper> {
public:
    VIRTUAL_METHOD(void, setResult, WIN32_LINUX(14, 11), (void* params, int result), (params, result))
};

}
