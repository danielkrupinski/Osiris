#pragma once

#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct SoundEmitter; }

class SoundEmitter : public VirtualCallableFromPOD<SoundEmitter, pod::SoundEmitter> {
public:
    VIRTUAL_METHOD(const char*, getSoundName, 46, (int index), (index))
};

}
