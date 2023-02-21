#pragma once

#include "../Memory.h"
#include "VirtualMethod.h"

namespace csgo
{

struct KeyValuesPOD;

struct KeyValues : VirtualCallableFromPOD<KeyValues, KeyValuesPOD> {
    KeyValues(VirtualCallableFromPOD base, const Memory& memory)
    : VirtualCallableFromPOD{ base }, memory{ memory }
    {
    }

    KeyValuesPOD* findKey(const char* keyName, bool create) noexcept;
    void setString(const char* keyName, const char* value) noexcept;

private:
    const Memory& memory;
};

}
