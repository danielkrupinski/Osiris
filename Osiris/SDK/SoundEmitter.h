#pragma once

#include "VirtualMethod.h"

class SoundEmitter {
public:
    VIRTUAL_METHOD(const char*, getSoundName, 46, (int index), (this, index))
};
