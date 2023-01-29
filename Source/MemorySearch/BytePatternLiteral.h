#pragma once

#include "BytePatternStorage.h"

template <BytePatternStorage storage>
consteval auto operator"" _pat()
{
    return BytePatternStorage<storage.size>{ storage };
}
