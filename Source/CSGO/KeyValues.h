#pragma once

#include "Helpers/KeyValuesFunctions.h"
#include "VirtualMethod.h"

namespace csgo
{

struct KeyValuesPOD;

struct KeyValues : VirtualCallableFromPOD<KeyValues, KeyValuesPOD> {
    KeyValues(VirtualCallableFromPOD base, KeyValuesFunctions functions)
    : VirtualCallableFromPOD{ base }, functions{ functions }
    {
    }

    KeyValues findKey(const char* keyName, bool create) const noexcept;
    void setString(const char* keyName, const char* value) const noexcept;

private:
    KeyValuesFunctions functions;
};

}
