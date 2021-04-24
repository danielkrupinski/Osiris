#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class SoundEmitter {
public:
    INCONSTRUCTIBLE(SoundEmitter)

    VIRTUAL_METHOD(const char*, getSoundName, 46, (int index), (this, index))
};
