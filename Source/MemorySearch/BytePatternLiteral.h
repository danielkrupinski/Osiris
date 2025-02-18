#pragma once

#include <array>

#include "BytePatternStorage.h"
#include "BytePatternView.h"

template <BytePatternStorage Storage>
auto operator ""_pat()
{
    static constexpr std::array<char, Storage.size> pattern{[]{
        std::array<char, Storage.size> truncatedPattern;
        std::copy_n(Storage.pattern.begin(), Storage.size, truncatedPattern.begin());
        return truncatedPattern;
    }()};

    return BytePatternView<Storage.size>{pattern};
}
