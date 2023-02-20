#include "KeyValues.h"
#include "../Memory.h"

KeyValues* KeyValues::findKey(const Memory& memory, const char* keyName, bool create) noexcept
{
    return memory.keyValuesFindKey(this, keyName, create);
}

void KeyValues::setString(const Memory& memory, const char* keyName, const char* value) noexcept
{
    if (const auto key = findKey(memory, keyName, true))
        memory.keyValuesSetString(key, value);
}
