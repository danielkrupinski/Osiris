#pragma once

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

    [[nodiscard]] consteval auto abs(std::uint8_t offsetToTheNextInstruction = 4) noexcept
    {
        operation = CodePatternOperation::Abs;
        offsetToNextInstruction = offsetToTheNextInstruction;
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
    std::uint8_t offsetToNextInstruction{};
};
