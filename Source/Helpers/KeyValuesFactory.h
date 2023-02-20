#pragma once

#include <cstdint>
#include <MemorySearch/PatternFinder.h>
#include <Platform/Macros/IsPlatform.h>

class KeyValues;

struct KeyValuesFactory {
    explicit KeyValuesFactory(std::uintptr_t fromStringFn)
        : fromStringFn{fromStringFn}
    {
    }

    KeyValues* operator()(const char* name, const char* value)
    {
#if IS_WIN32()
        const auto keyValuesFromString = fromStringFn;
        KeyValues *keyValues;
        __asm {
                push 0
                mov edx, value
                mov ecx, name
                call keyValuesFromString
                add esp, 4
                mov keyValues, eax
        }
        return keyValues;
#else
        return reinterpret_cast<KeyValues *(*)(const char*, const char*, const char**)>(fromStringFn)(name, value, nullptr);
#endif
    }

private:
    std::uintptr_t fromStringFn;
};

[[nodiscard]] KeyValuesFactory createKeyValuesFactory(const PatternFinder &clientPatternFinder);
