#pragma once

struct CString {
    bool skipPrefix(const char* prefix) noexcept
    {
        while (*string == *prefix && *prefix != '\0') {
            ++string;
            ++prefix;
        }
        return *prefix == '\0';
    }

    const char* string;
};
