#pragma once

#include <cassert>
#include <cstddef>
#include <optional>
#include <span>
#include <string_view>

class BytePattern {
public:
    constexpr BytePattern(std::string_view pattern, std::optional<char> wildcardChar = {}) noexcept
        : pattern{pattern}
        , wildcardChar{wildcardChar}
    {
        assert(!pattern.empty());
    }

    [[nodiscard]] BytePattern withoutFirstAndLastChar() const noexcept
    {
        if (pattern.size() > 2)
            return BytePattern{ std::string_view{ pattern.data() + 1, pattern.size() - 2 }, wildcardChar };
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

    [[nodiscard]] std::string_view raw() const noexcept
    {
        return pattern;
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

    [[nodiscard]] std::optional<char> getWildcardChar() const noexcept
    {
        return wildcardChar;
    }

private:
    BytePattern() = default;

    std::string_view pattern;
    std::optional<char> wildcardChar;
};
