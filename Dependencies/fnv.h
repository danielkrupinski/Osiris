/*

MIT License

Copyright (c) 2019 Daniel Krupiński

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <cstdint>

namespace fnv {
    constexpr uint32_t offsetBasis = 0x811c9dc5;
    constexpr uint32_t prime = 0x1000193;

    constexpr uint32_t hash(const char* str, const uint32_t value = offsetBasis) noexcept
    {
        return *str ? hash(str + 1, (value ^ *str) * static_cast<unsigned long long>(prime)) : value;
    }

    constexpr uint32_t hashRuntime(const char* str) noexcept
    {
        auto value = offsetBasis;

        while (*str) {
            value ^= *str++;
            value *= prime;
        }
        return value;
    }
}
