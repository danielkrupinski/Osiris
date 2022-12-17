#pragma once

#include "UtlMemory.h"

namespace csgo
{

struct UtlString {
    UtlMemory<char> buffer;
    int length;

    const char* data() const noexcept { return buffer.memory; }
};

}
