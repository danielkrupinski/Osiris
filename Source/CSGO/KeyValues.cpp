#include "KeyValues.h"
#include <RetSpoof/FunctionInvoker.h>

csgo::KeyValues csgo::KeyValues::findKey(const char* keyName, bool create) const noexcept
{
    return csgo::KeyValues::from(getInvoker(), FunctionInvoker{ getInvoker(), functions.findKey }(getPOD(), keyName, create), functions);
}

void csgo::KeyValues::setString(const char* keyName, const char* value) const noexcept
{
    if (const auto key = findKey(keyName, true); key.getPOD() != nullptr)
        FunctionInvoker{ getInvoker(), functions.setString }(key.getPOD(), value);
}
