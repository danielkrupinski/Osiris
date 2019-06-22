#pragma once

#include "Utils.h"

class SoundEmitter {
public:
    constexpr auto getSoundName(int index) noexcept
    {
        return callVirtualMethod<const char*, int>(this, 46, index);
    }
};
