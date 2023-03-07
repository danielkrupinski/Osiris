#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct SoundEmitterPOD;

struct SoundEmitter : VirtualCallableFromPOD<SoundEmitter, SoundEmitterPOD> {
    VIRTUAL_METHOD(const char*, getSoundName, 46, (int index), (index))
};

}
