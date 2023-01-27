#pragma once

#include <MemorySearch/BytePattern.h>
#include <MemorySearch/PatternFinder.h>
#include <Utils/SafeAddress.h>

namespace helpers
{

struct PatternFinder : public ::PatternFinder {
    using ::PatternFinder::PatternFinder;

    [[nodiscard]] SafeAddress operator()(BytePattern pattern) const noexcept;
    [[nodiscard]] SafeAddress operator()(BytePattern pattern, OffsetHint offsetHint) const noexcept;
};

}
