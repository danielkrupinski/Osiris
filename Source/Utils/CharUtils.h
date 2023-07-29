#pragma once

namespace utils
{

[[nodiscard]] constexpr wchar_t toUpper(wchar_t c) noexcept
{
    if (c >= L'a' && c <= L'z')
        return c - L'a' + L'A';
    return c;
}

}
