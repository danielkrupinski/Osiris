#include <cstddef>
#include <cstring>

extern "C" int _fltused{ 0x9875 };

namespace intrinsics
{

[[nodiscard]] inline int memcmp(const unsigned char* lhs, const unsigned char* rhs, std::size_t count) noexcept
{
    while (count-- > 0) {
        if (*lhs++ != *rhs++)
            return lhs[-1] < rhs[-1] ? -1 : 1;
    }
    return 0;
}

inline std::byte* memcpy(std::byte* dest, const std::byte* src, std::size_t count) noexcept
{
    while (count--)
        *dest++ = *src++;
    return dest;
}

inline unsigned char* memset(unsigned char* const dest, unsigned char ch, std::size_t count) noexcept
{
    unsigned char* ptr = dest;
    while (count-- > 0)
        *ptr++ = ch;
    return dest;
}

[[nodiscard]] inline int strcmp(const unsigned char* s1, const unsigned char* s2) noexcept
{
    while (*s1 == *s2++) {
        if (*s1++ == '\0')
            return 0;
    }   
    return *s1 - s2[-1];
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
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);
    if (d < s) {
        while (count--)
            *d++ = *s++;
    }
    else {
        const char* lasts = s + (count - 1);
        char* lastd = d + (count - 1);
        while (count--)
            *lastd-- = *lasts--;
    }
    return dest;
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
