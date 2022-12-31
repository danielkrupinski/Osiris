#pragma once

#include <MemorySearch/PatternFinder.h>
#include <Utils/SafeAddress.h>

namespace helpers
{

struct PatternFinder : public ::PatternFinder {
    using ::PatternFinder::PatternFinder;

    [[nodiscard]] SafeAddress operator()(std::string_view pattern) const noexcept;
    [[nodiscard]] SafeAddress operator()(std::string_view pattern, OffsetHint offsetHint) const noexcept;
};

}
