#pragma once

#include "Inconstructible.h"
#include "Platform.h"
#include "VirtualMethod.h"

class PanoramaMarshallHelper {
public:
    INCONSTRUCTIBLE(PanoramaMarshallHelper)

    VIRTUAL_METHOD(void, setResult, WIN32_LINUX(14, 11), (void* params, int result), (this, params, result))
};
