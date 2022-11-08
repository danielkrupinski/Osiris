#pragma once

#include <SafeAddress.h>
#include <Utils/PatternFinder.h>

namespace helpers
{

struct PatternFinder : public ::PatternFinder {
    using ::PatternFinder::PatternFinder;

    [[nodiscard]] SafeAddress operator()(std::string_view pattern) const noexcept;
};

}
