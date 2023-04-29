#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <system_error>
#include <span>
#include <string_view>
#include <utility>

class StringBuilder {
public:
    explicit StringBuilder(std::span<char> buffer)
        : buffer{ buffer }
    {
        assert(!buffer.empty());
    }

    template <typename... Args>
    StringBuilder& put(Args&&... args)
    {
        (putSingle(std::forward<Args>(args)), ...);
        return *this;
    }

    StringBuilder& putHex(std::integral auto value)
    {
        putSingleWithBase(value, 16);
        return *this;
    }

    [[nodiscard]] std::string_view string() const noexcept
    {
        return std::string_view{ buffer.data(), writePosition };
    }

    [[nodiscard]] const char* cstring() noexcept
    {
        *writePosition = '\0';
        return buffer.data();
    }

private:
    template <std::invocable<StringBuilder&> Invocable>
    void putSingle(Invocable invocable)
    {
        invocable(*this);
    }

    void putSingle(std::integral auto value)
    {
        putSingleWithBase(value, 10);
    }

    void putSingleWithBase(std::integral auto value, int base)
    {
        const auto result = std::to_chars(writePosition, writeEndPosition(), value, base);
        if (result.ec == std::errc{})
            writePosition = result.ptr;
    }

    void putSingle(char c)
    {
        if (writePosition != writeEndPosition()) {
            *writePosition = c;
            ++writePosition;
        }
    }

    void putSingle(std::string_view str)
    {
        const auto sizeToCopy = (std::min)(str.size(), unusedSpace());
        writePosition = std::copy_n(str.data(), sizeToCopy, writePosition);
    }

    [[nodiscard]] char* writeEndPosition() noexcept
    {
        return &buffer[capacityExcludingNullTerminator()];
    }

    [[nodiscard]] std::size_t unusedSpace() noexcept
    {
        return writeEndPosition() - writePosition;
    }

    [[nodiscard]] std::size_t capacityExcludingNullTerminator() const noexcept
    {
        return buffer.size() - 1;
    }

    std::span<char> buffer;
    char* writePosition = buffer.data();
};

template <std::size_t Capacity>
struct StringBuilderStorage {
    StringBuilderStorage()
    {
        // prevents zeroing the buffer in zero initialization
    }

    StringBuilder builder()
    {
        return StringBuilder{ buffer };
    }

private:
    std::array<char, Capacity> buffer;
};
