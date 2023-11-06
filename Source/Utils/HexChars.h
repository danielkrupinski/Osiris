#pragma once

#include <array>
#include <cstdint>
#include <optional>

[[nodiscard]] constexpr std::uint8_t hexDigitToDecimal(char hexDigit) noexcept
{
    if (hexDigit >= '0' && hexDigit <= '9')
        return hexDigit - '0';
    if (hexDigit >= 'A' && hexDigit <= 'F')
        return hexDigit - 'A' + 10;
    return 0xFF;
}

[[nodiscard]] constexpr bool isValidOneDigitHexNumber(std::uint8_t number) noexcept
{
    return (number & 0xF0) == 0;
}

constexpr auto hexCharsInByte = 2;

[[nodiscard]] constexpr std::optional<char> hexCharsToByte(std::array<char, hexCharsInByte> chars)
{
    const auto highFourBits = hexDigitToDecimal(chars[0]);
    const auto lowFourBits = hexDigitToDecimal(chars[1]);

    if (isValidOneDigitHexNumber(highFourBits) && isValidOneDigitHexNumber(lowFourBits))
        return static_cast<char>(highFourBits << 4 | lowFourBits);
    return {};
}
