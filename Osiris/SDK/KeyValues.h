#pragma once

class KeyValues {
public:
    static KeyValues* fromString(const char* name, const char* value) noexcept;
    KeyValues* findKey(const char* keyName, bool create) noexcept;
};
