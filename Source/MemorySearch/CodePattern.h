#pragma once

#include <cstddef>
#include <cstdint>

#include "BytePatternStorage.h"
#include "CodePatternOperation.h"

template <std::size_t N>
struct CodePattern {
    consteval CodePattern(const char (&patternString)[N]) noexcept
        : storage{patternString}
    {
    }

    [[nodiscard]] consteval auto add(std::uint8_t offsetToAdd) noexcept
    {
        offset = offsetToAdd;
        return *this;
    }

    [[nodiscard]] consteval auto abs(std::uint8_t offsetToNextInstruction = 4) noexcept
    {
        switch (offsetToNextInstruction) {
        case 4: operation = CodePatternOperation::Abs4; break;
        case 5: operation = CodePatternOperation::Abs5; break;
        default: error("unsupported offset");
        }
        return *this;
    }

    [[nodiscard]] consteval auto read() noexcept
    {
        operation = CodePatternOperation::Read;
        return *this;
    }

    BytePatternStorage<N> storage;
    std::uint8_t offset{};
    CodePatternOperation operation{CodePatternOperation::None};

private:
    void error(const char*);
};
