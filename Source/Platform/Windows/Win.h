#pragma once

#include <Platform/Macros/IsPlatform.h>
#include <Utils/CharUtils.h>
#include <Utils/Pad.h>

namespace win
{

struct ListEntry {
    ListEntry* fLink;
    ListEntry* bLink;
};

static_assert(IS_WIN64());

struct PebLdrData {
    PAD(0x10);
    ListEntry inLoadOrderModuleList;
};

struct Peb {
    PAD(0x18);
    PebLdrData* ldr;
};

struct UnicodeString {
    unsigned short length;
    unsigned short maximumLength;
    wchar_t* buffer;

    [[nodiscard]] unsigned short lengthInChars() const noexcept
    {
        return length / sizeof(wchar_t);
    }

    [[nodiscard]] bool equalsCaseInsensitive(const char* otherString) const noexcept
    {
        for (std::size_t i = 0; i < lengthInChars(); ++i) {
            if (utils::toUpper(buffer[i]) != utils::toUpper(otherString[i]) || otherString[i] == '\0')
                return false;
        }
        return true;
    }
};

struct LdrDataTableEntry {
    ListEntry inLoadOrderLinks;
    ListEntry inMemoryOrderLinks;
    ListEntry inInitializationOrderLinks;
    void* dllBase;
    void* entryPoint;
    unsigned long sizeOfImage;
    UnicodeString fullDllName;
    UnicodeString baseDllName;
};

}
