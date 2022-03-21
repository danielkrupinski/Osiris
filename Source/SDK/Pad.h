#pragma once

#include <cstddef>

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)

#define PAD(size) \
private: \
    [[maybe_unused]] std::byte PAD_NAME(__LINE__) [size]; \
public:
