#pragma once

#include <concepts>
#include <cstddef>
#include <limits>
#include <string_view>

class StringParser {
public:
    explicit StringParser(const char* string) noexcept
        : string{ string }
    {
    }

    [[nodiscard]] std::string_view getLine(char delimiter) noexcept
    {
        const auto begin = string;
        std::size_t length = 0;
        while (*string != '\0' && *string++ != delimiter)
            ++length;
        return { begin, length };
    }

    [[nodiscard]] char getChar() noexcept
    {
        if (*string != '\0')
            return *string++;
        return '\0';
    }

    template <std::integral IntegralType>
    bool parseInt(IntegralType& result) noexcept
    {
        static_assert(std::is_unsigned_v<IntegralType>);

        IntegralType parsedInteger{};
        bool parseSuccessful = false;

        std::size_t currentDigit = 0;
        while (*string >= '0' && *string <= '9') {
            if (currentDigit < std::numeric_limits<IntegralType>::digits10) {
                parsedInteger *= 10;
                parsedInteger += (*string - '0');
                ++currentDigit;
                parseSuccessful = true;
            } else {
                parseSuccessful = false;
            }

            ++string;
        }

        if (parseSuccessful)
            result = parsedInteger;

        return parseSuccessful;
    }

    template <std::floating_point FloatType>
    bool parseFloat(FloatType& result) noexcept
    {
        FloatType parsedFloat{};
        bool parseSuccessful = false;

        if (*string == '-') {
            // unsupported
            return false;
        }

        while (*string >= '0' && *string <= '9') {
            parsedFloat *= 10.0f;
            parsedFloat += static_cast<FloatType>(*string - '0');
            parseSuccessful = true;
            ++string;
        }

        if (*string == '.')
            ++string;

        FloatType fraction = 1.0f;
        while (*string >= '0' && *string <= '9') {
            fraction /= 10.0f;
            parsedFloat += static_cast<FloatType>(*string - '0') * fraction;
            parseSuccessful = true;
            ++string;
        }

        if (parseSuccessful)
            result = parsedFloat;

        return parseSuccessful;
    }

private:
    const char* string;
};
