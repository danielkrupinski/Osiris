#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class PanoramaMarshallHelper {
public:
    INCONSTRUCTIBLE(PanoramaMarshallHelper)

    VIRTUAL_METHOD(void, setResult, 14, (void* params, int result), (this, params, result))
};
