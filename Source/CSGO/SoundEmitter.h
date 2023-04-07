#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct SoundEmitterPOD;

struct SoundEmitter : GameClass<SoundEmitter, SoundEmitterPOD> {
    VIRTUAL_METHOD(const char*, getSoundName, 46, (int index), (index))
};

}
