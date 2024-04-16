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
    }

    template <typename T>
    [[nodiscard]] static BytePattern ofObject(const T& object) noexcept
    {
        return BytePattern{std::string_view{reinterpret_cast<const char*>(&object), sizeof(T)}};
    }

    template <typename T>
    static BytePattern ofObject(const T&&) = delete;

    [[nodiscard]] std::size_t indexOfFirstNonWildcardChar() const noexcept
    {
        if (wildcardChar)
            return pattern.find_first_not_of(*wildcardChar);
        return 0;
    }

    [[nodiscard]] std::size_t indexOfLastNonWildcardChar() const noexcept
    {
        if (wildcardChar)
            return pattern.find_last_not_of(*wildcardChar);
        return pattern.size() - 1;
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
    std::string_view pattern;
    std::optional<char> wildcardChar;
};
