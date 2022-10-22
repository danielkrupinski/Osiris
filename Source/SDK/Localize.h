#pragma once

#include <string>

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Localize : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2(const wchar_t*, find, 11, (const char* tokenName), (tokenName))
    VIRTUAL_METHOD2(const wchar_t*, findSafe, 12, (const char* tokenName), (tokenName))
    VIRTUAL_METHOD2(int, convertAnsiToUnicode, 15, (const char* ansi, wchar_t* unicode, int unicodeBufferSizeInBytes), (ansi, unicode, unicodeBufferSizeInBytes))
    VIRTUAL_METHOD2(int, convertUnicodeToAnsi, 16, (const wchar_t* unicode, char* ansi, int ansiBufferSize), (unicode, ansi, ansiBufferSize))
    VIRTUAL_METHOD2(const char*, findAsUTF8, 47, (const char* tokenName), (tokenName))
};

template <std::size_t BufferSize = 4096>
class ToUtf8Converter {
public:
    ToUtf8Converter(Localize localize) : localize{ localize } {}
    
    [[nodiscard]] std::string_view convertUnicodeToAnsi(const wchar_t* string)
    {
        localize.convertUnicodeToAnsi(string, buffer.data(), buffer.size());
        buffer.back() = '\0';
        return buffer.data();
    }

private:
    Localize localize;
    std::array<char, BufferSize> buffer;
};
