#pragma once

#include <cstddef>

#define PAD(size) \
private: \
    std::byte _pad_##size[size]; \
public:
