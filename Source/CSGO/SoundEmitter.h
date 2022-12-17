#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct SoundEmitterPOD;

class SoundEmitter : public VirtualCallableFromPOD<SoundEmitter, SoundEmitterPOD> {
public:
    VIRTUAL_METHOD(const char*, getSoundName, 46, (int index), (index))
};

}
