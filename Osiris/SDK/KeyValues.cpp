#include "KeyValues.h"
#include "../Memory.h"

KeyValues* KeyValues::fromString(const char* name, const char* value) noexcept
{
    const auto keyValuesFromString = memory->keyValuesFromString;
    KeyValues* keyValues;
    __asm {
        push 0
        mov edx, value
        mov ecx, name
        call keyValuesFromString
        add esp, 4
        mov keyValues, eax
    }
    return keyValues;
}
