#pragma once

#include <cstdint>
#include <BytePatterns/ClientPatternFinder.h>
#include <CSGO/Helpers/KeyValuesFunctions.h>
#include <CSGO/KeyValues.h>
#include <MemorySearch/PatternFinder.h>
#include <Platform/Macros/IsPlatform.h>
#include <RetSpoof/RetSpoofInvoker.h>

struct KeyValuesFactory {
    explicit KeyValuesFactory(RetSpoofInvoker retSpoofInvoker, std::uintptr_t fromStringFn, KeyValuesFunctions keyValuesFunctions)
        : retSpoofInvoker{ retSpoofInvoker }, fromStringFn{fromStringFn}, keyValuesFunctions{ keyValuesFunctions }
    {
    }

    csgo::KeyValues operator()(const char* name, const char* value)
    {
#if IS_WIN32()
        const auto keyValuesFromString = fromStringFn;
        csgo::KeyValuesPOD* keyValues;
        __asm {
                push 0
                mov edx, value
                mov ecx, name
                call keyValuesFromString
                add esp, 4
                mov keyValues, eax
        }
        return csgo::KeyValues::from(retSpoofInvoker, keyValues, keyValuesFunctions);
#else
        return csgo::KeyValues::from(retSpoofInvoker, reinterpret_cast<csgo::KeyValuesPOD*(*)(const char*, const char*, const char**)>(fromStringFn)(name, value, nullptr), keyValuesFunctions);
#endif
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    std::uintptr_t fromStringFn;
    KeyValuesFunctions keyValuesFunctions;
};

[[nodiscard]] KeyValuesFactory createKeyValuesFactory(RetSpoofInvoker retSpoofInvoker, const ClientPatternFinder& clientPatternFinder);
