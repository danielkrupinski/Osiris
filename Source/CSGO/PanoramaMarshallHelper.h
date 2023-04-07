#pragma once

#include <Platform/Macros/PlatformSpecific.h>
#include "VirtualMethod.h"

namespace csgo
{

struct PanoramaMarshallHelperPOD;

struct PanoramaMarshallHelper : GameClass<PanoramaMarshallHelper, PanoramaMarshallHelperPOD> {
    VIRTUAL_METHOD(void, setResult, WIN32_LINUX(14, 11), (void* params, int result), (params, result))
};

}
