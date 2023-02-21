#include "KeyValues.h"
#include <RetSpoof/FunctionInvoker.h>

csgo::KeyValuesPOD* csgo::KeyValues::findKey(const char* keyName, bool create) const noexcept
{
    return FunctionInvoker{ getInvoker(), functions.findKey }(getPOD(), keyName, create);
}

void csgo::KeyValues::setString(const char* keyName, const char* value) const noexcept
{
    if (const auto key = findKey(keyName, true))
        FunctionInvoker{ getInvoker(), functions.setString }(key, value);
}
