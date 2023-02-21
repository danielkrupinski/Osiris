#include "KeyValues.h"
#include "../Memory.h"

csgo::KeyValuesPOD* csgo::KeyValues::findKey(const char* keyName, bool create) noexcept
{
    return memory.keyValuesFindKey(getPOD(), keyName, create);
}

void csgo::KeyValues::setString(const char* keyName, const char* value) noexcept
{
    if (const auto key = findKey(keyName, true))
        memory.keyValuesSetString(key, value);
}
