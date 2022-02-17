#pragma once

#include <string>

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Localize {
public:
    INCONSTRUCTIBLE(Localize)

    VIRTUAL_METHOD(const wchar_t*, find, 11, (const char* tokenName), (this, tokenName))
    VIRTUAL_METHOD(const wchar_t*, findSafe, 12, (const char* tokenName), (this, tokenName))
    VIRTUAL_METHOD(int, convertAnsiToUnicode, 15, (const char* ansi, wchar_t* unicode, int unicodeBufferSizeInBytes), (this, ansi, unicode, unicodeBufferSizeInBytes))
    VIRTUAL_METHOD(int, convertUnicodeToAnsi, 16, (const wchar_t* unicode, char* ansi, int ansiBufferSize), (this, unicode, ansi, ansiBufferSize))
    VIRTUAL_METHOD(const char*, findAsUTF8, 47, (const char* tokenName), (this, tokenName))
};

template <std::size_t BufferSize = 4096>
class ToUtf8Converter {
public:
    ToUtf8Converter(Localize& localize) : localize{ localize } {}
    
    [[nodiscard]] std::string_view convertUnicodeToAnsi(const wchar_t* string)
    {
        localize.convertUnicodeToAnsi(string, buffer.data(), buffer.size());
        buffer.back() = '\0';
        return buffer.data();
    }

private:
    Localize& localize;
    std::array<char, BufferSize> buffer;
};
