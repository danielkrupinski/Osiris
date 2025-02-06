#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <optional>
#include <string_view>
#include <utility>

#include "PatternStringWildcard.h"
#include <Utils/HexChars.h>

enum class BytePatternConverterError {
    NoError,
    WildcardUsedAsByte,
    UnexpectedChar
};

template <std::size_t N>
    requires (N > 0)
struct BytePatternConverter {
    explicit constexpr BytePatternConverter(const char(&patternString)[N])
    {
        std::ranges::copy(patternString, buffer.begin());
    }

    using Error = BytePatternConverterError;

    [[nodiscard]] constexpr std::pair<std::string_view, Error> operator()()
    {
        while (canConvert())
            convertNextSequence();
        return conversionResult();
    }

private:
    [[nodiscard]] constexpr bool canConvert() noexcept
    {
        return hasCharsToConvert() && noErrorOccured();
    }

    [[nodiscard]] constexpr bool noErrorOccured() const noexcept
    {
        return error == Error::NoError;
    }

    [[nodiscard]] constexpr std::pair<std::string_view, Error> conversionResult()
    {
        return { converted(), error };
    }

    constexpr void convertNextSequence()
    {
        spaceExpected ? convertSpace() : convertByteOrWildcard();
        toggleSpaceExpected();
    }

    constexpr void toggleSpaceExpected() noexcept
    {
        spaceExpected = !spaceExpected;
    }

    constexpr void convertByteOrWildcard()
    {
        isNextCharWildcard() ? convertWildcard() : convertByte();
    }

    [[nodiscard]] constexpr std::string_view converted() const
    {
        return std::string_view{ buffer.data(), writePosition };
    }

    [[nodiscard]] static constexpr bool isWildcardChar(char c) noexcept
    {
        return c == kPatternStringWildcard;
    }

    [[nodiscard]] constexpr bool isNextCharWildcard() const
    {
        return isWildcardChar(peekNextChar());
    }

    [[nodiscard]] constexpr bool isNextCharSpace() const
    {
        return isWhitespace(peekNextChar());
    }

    [[nodiscard]] static constexpr bool isWhitespace(char c) noexcept
    {
        return c == ' ' || c == '\0';
    }

    constexpr void setUnexpectedCharError() noexcept
    {
        error = Error::UnexpectedChar;
    }

    constexpr void setWildcardUsedAsByteError() noexcept
    {
        error = Error::WildcardUsedAsByte;
    }

    constexpr void convertSpace() noexcept
    {
        isNextCharSpace() ? advanceReadPosition() : setUnexpectedCharError();
    }

    [[nodiscard]] constexpr std::optional<char> readAndConvertByte() noexcept
    {
        const auto read = readNextByte();
        return hexCharsToByte({ read[0], read[1] });
    }

    constexpr void convertByte() noexcept
    {
        canConvertByte() ? performByteConversion() : setUnexpectedCharError();
    }

    constexpr void performByteConversion()
    {
        const auto convertedByte = readAndConvertByte(); 
        convertedByte.has_value() ? handleConvertedByte(*convertedByte) : setUnexpectedCharError();
    }

    constexpr void handleConvertedByte(char convertedByte)
    {
        !isWildcardChar(convertedByte) ? putChar(convertedByte) : setWildcardUsedAsByteError();
    }

    constexpr void convertWildcard()
    {
        assert(isNextCharWildcard());

        advanceReadPosition();
        putChar(kPatternStringWildcard);
    }

    [[nodiscard]] constexpr bool canConvertByte() const noexcept
    {
        return charsToConvert() >= hexCharsInByte;
    }

    [[nodiscard]] constexpr std::size_t charsToConvert() const noexcept
    {
        return buffer.size() - readPosition;
    }

    [[nodiscard]] constexpr bool hasCharsToConvert() const noexcept
    {
        return charsToConvert() > 0;
    }

    [[nodiscard]] constexpr std::string_view readNextByte() noexcept
    {
        const std::string_view nextByte{ buffer.data() + readPosition, hexCharsInByte };
        readPosition += hexCharsInByte;
        return nextByte;
    }

    [[nodiscard]] constexpr char peekNextChar() const noexcept
    {
        assert(hasCharsToConvert());
        return buffer[readPosition];
    }

    constexpr void advanceReadPosition() noexcept
    {
        ++readPosition;
    }
    
    constexpr void putChar(char convertedChar) noexcept
    {
        buffer[writePosition] = convertedChar;
        ++writePosition;
    }

    std::array<char, N> buffer;
    std::size_t readPosition = 0;
    std::size_t writePosition = 0;
    bool spaceExpected = false;
    Error error = Error::NoError;
};
