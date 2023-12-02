#include <cstddef>
#include <cstring>

extern "C" int _fltused{ 0x9875 };

namespace intrinsics
{

[[nodiscard]] inline int memcmp(const unsigned char* lhs, const unsigned char* rhs, std::size_t count) noexcept
{
    for (std::size_t i = 0; i < count; ++i) {
        if (lhs[i] != rhs[i])
            return lhs[i] < rhs[i] ? -1 : 1;
    }
    return 0;
}

inline std::byte* memcpy(std::byte* dest, const std::byte* src, std::size_t count) noexcept
{
    for (std::size_t i = 0; i < count; ++i)
        dest[i] = src[i];
    return dest;
}

inline unsigned char* memset(unsigned char* const dest, unsigned char ch, std::size_t count) noexcept
{
    for (std::size_t i = 0; i < count; ++i)
        dest[i] = ch;
    return dest;
}

[[nodiscard]] inline int strcmp(const unsigned char* s1, const unsigned char* s2) noexcept
{
    while (*s1 == *s2) {
        if (*s1 == '\0')
            return 0;
        ++s1;
        ++s2;
    }

    return *s1 - *s2;
}

inline unsigned char* memmove(unsigned char* dest, const unsigned char* src, std::size_t count) noexcept
{
    if (dest < src) {
        for (std::size_t i = 0; i < count; ++i)
            dest[i] = src[i];
    } else {
        for (std::size_t i = count; i > 0; --i)
            dest[i - 1] = src[i - 1];
    }
    return dest;
}

inline std::size_t strlen(const char* str) noexcept
{
    std::size_t length = 0;
    while (*str) {
        ++length;
        ++str;
    }
    return length;
}

inline const unsigned char* memchr(const unsigned char* ptr, unsigned char ch, std::size_t count) noexcept
{
    for (std::size_t i = 0; i < count; ++i) {
        if (ptr[i] == ch)
            return &ptr[i];
    }
    return nullptr;
}

}

#pragma function(memcpy)
void* memcpy(void* dest, const void* src, size_t count)
{
    return intrinsics::memcpy(static_cast<std::byte*>(dest), static_cast<const std::byte*>(src), count);
}

#pragma function(memmove)
void* memmove(void* dest, const void* src, size_t count)
{
    return intrinsics::memmove(static_cast<unsigned char*>(dest), static_cast<const unsigned char*>(src), count);
}

#pragma function(memcmp)
int memcmp(const void* s1, const void* s2, std::size_t n)
{
    return intrinsics::memcmp(static_cast<const unsigned char*>(s1), static_cast<const unsigned char*>(s2), n);
}

#pragma function(memset)
void* memset(void* dest, int ch, size_t count)
{
    return intrinsics::memset(static_cast<unsigned char*>(dest), static_cast<unsigned char>(ch), count);
}

#pragma function(strcmp)
int strcmp(const char* s1, const char* s2)
{
    return intrinsics::strcmp(reinterpret_cast<const unsigned char*>(s1), reinterpret_cast<const unsigned char*>(s2));
}

#pragma function(strlen)
std::size_t strlen(const char* str)
{
    return intrinsics::strlen(str);
}

#pragma function(memchr)
const void* memchr(const void* ptr, int ch, std::size_t count)
{
    return intrinsics::memchr(reinterpret_cast<const unsigned char*>(ptr), static_cast<unsigned char>(ch), count);
}
