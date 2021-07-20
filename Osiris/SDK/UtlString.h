#pragma once

#include "UtlMemory.h"

struct UtlString {
    UtlMemory<char> buffer;
    int length;

    const char* data() const noexcept { return buffer.memory; }
};
