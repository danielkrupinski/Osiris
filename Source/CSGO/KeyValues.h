#pragma once

#include "../Memory.h"

class KeyValues {
public:
    KeyValues* findKey(const Memory& memory, const char* keyName, bool create) noexcept;
    void setString(const Memory& memory, const char* keyName, const char* value) noexcept;
};
