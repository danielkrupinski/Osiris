#pragma once

#include <cassert>
#include <cstddef>
#include <span>
#include <string_view>

class BytePattern {
public:
    static constexpr auto wildcardChar = '?';

    explicit(false) BytePattern(std::string_view pattern)
        : pattern{ pattern }
    {
        assert(!pattern.empty());
        assert(pattern.front() != wildcardChar && pattern.back() != wildcardChar);
    }

    [[nodiscard]] BytePattern withoutFirstAndLastChar() const noexcept
    {
        if (pattern.size() > 2)
            return std::string_view{ pattern.data() + 1, pattern.size() - 2 };
        return {};
    }

    [[nodiscard]] std::size_t length() const noexcept
    {
        return pattern.length();
    }

    [[nodiscard]] char front() const noexcept
    {
        return pattern.front();
    }

    [[nodiscard]] char back() const noexcept
    {
        return pattern.back();
    }

    [[nodiscard]] bool matches(std::span<const std::byte> bytes) const noexcept
    {
        assert(bytes.size() == pattern.size());

        for (std::size_t i = 0; i < bytes.size(); ++i) {
            if (std::to_integer<char>(bytes[i]) != pattern[i] && pattern[i] != wildcardChar)
                return false;
        }
        return true;
    }

private:
    BytePattern() = default;

    std::string_view pattern;
};
