#pragma once

#include "Helpers/KeyValuesFunctions.h"
#include "VirtualMethod.h"

namespace csgo
{

struct KeyValuesPOD;

struct KeyValues : GameClass<KeyValues, KeyValuesPOD> {
    KeyValues(GameClass base, KeyValuesFunctions functions)
    : GameClass{ base }, functions{ functions }
    {
    }

    KeyValues findKey(const char* keyName, bool create) const noexcept;
    void setString(const char* keyName, const char* value) const noexcept;

private:
    KeyValuesFunctions functions;
};

}
