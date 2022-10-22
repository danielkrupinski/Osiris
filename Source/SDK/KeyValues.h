#pragma once

#include "../Memory.h"

class KeyValues {
public:
    static KeyValues* fromString(const Memory& memory, const char* name, const char* value) noexcept;
    KeyValues* findKey(const Memory& memory, const char* keyName, bool create) noexcept;
    void setString(const Memory& memory, const char* keyName, const char* value) noexcept;
};
