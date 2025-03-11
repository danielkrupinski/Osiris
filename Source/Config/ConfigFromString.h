#pragma once

#include <span>

#include "ConfigStringConversionState.h"

class ConfigFromString {
public:
    ConfigFromString(std::span<const char8_t> buffer, ConfigStringConversionState& conversionState) noexcept
        : buffer{buffer}
        , conversionState{conversionState}
    {
    }

    void beginRoot() noexcept
    {
        if (shouldReadMe() && skipWhitespaces() && readChar(u8'{'))
            increaseConversionNestingLevel();
        increaseNestingLevel();
    }

    [[nodiscard]] std::size_t endRoot() noexcept
    {
        if (shouldReadMe() && skipWhitespaces() && readChar(u8'}'))
            markConversionComplete();
        decreaseNestingLevel();
        conversionState.offset += readIndex;
        return readIndex;
    }

    void beginObject(const char8_t* key) noexcept
    {
        if (shouldReadMe()) {
            const auto previousReadIndex = readIndex;
            if (readUntilStartOfValue(key) && readChar(u8'{'))
                increaseConversionNestingLevel();
            else
                readIndex = previousReadIndex;
        }
        increaseNestingLevel();
    }

    void endObject() noexcept
    {
        if (shouldReadMe() && skipWhitespaces() && readChar(u8'}'))
            decreaseConversionNestingLevel();
        decreaseNestingLevel();
    }

    void boolean(const char8_t* key, auto&& valueSetter, auto&& /* valueGetter */) noexcept
    {
        if (bool value; parseBool(key, value))
            valueSetter(value);
    }

    void uint(const char8_t* key, auto&& valueSetter, auto&& /* valueGetter */) noexcept
    {
        if (std::uint64_t value; parseUint(key, value))
            valueSetter(value);
    }

private:
    [[nodiscard]] bool parseBool(const char8_t* key, bool& value) noexcept
    {
        bool parsed = false;
        if (shouldReadMe()) {
            const auto previousReadIndex = readIndex;
            if (readUntilStartOfValue(key) && parseBool(value)) {
                parsed = true;
                markElementReadAtCurrentNestingLevel();
            } else {
                readIndex = previousReadIndex;
            }
        }
        increaseIndexInNestingLevel();
        return parsed;
    }

    [[nodiscard]] bool parseUint(const char8_t* key, std::uint64_t& value) noexcept
    {
        bool parsed = false;
        if (shouldReadMe()) {
            const auto previousReadIndex = readIndex;
            if (readUntilStartOfValue(key) && parseUint(value)) {
                parsed = true;
                markElementReadAtCurrentNestingLevel();
            } else {
                readIndex = previousReadIndex;
            }
        }
        increaseIndexInNestingLevel();
        return parsed;
    }

    void markConversionComplete() noexcept
    {
        assert(conversionState.nestingLevel == 1);
        assert(conversionState.indexInNestingLevel[0] == 0);
        ++conversionState.indexInNestingLevel[--conversionState.nestingLevel];
    }

    void markElementReadAtCurrentNestingLevel() noexcept
    {
        conversionState.indexInNestingLevel[conversionState.nestingLevel] = 0;
    }

    [[nodiscard]] bool readUntilStartOfValue(const char8_t* key) noexcept
    {
        return readCommaAfterPreviousElement() && readKey(key) && skipWhitespaces();
    }

    [[nodiscard]] bool readCommaAfterPreviousElement() noexcept
    {
        if (conversionState.indexInNestingLevel[conversionState.nestingLevel] != config_params::kInvalidObjectIndex) {
            skipWhitespaces();
            return readChar(u8',');
        }
        return true;
    }

    [[nodiscard]] bool readKey(const char8_t* key) noexcept
    {
        return skipWhitespaces() && readChar(u8'"') && readString(key) && readChar(u8'"') && skipWhitespaces() && readChar(u8':');
    }

    void increaseNestingLevel() noexcept
    {
        assert(nestingLevel < config_params::kMaxNestingLevel);
        indexInNestingLevel[++nestingLevel] = 0;
    }

    void increaseConversionNestingLevel() noexcept
    {
        assert(conversionState.nestingLevel < config_params::kMaxNestingLevel);
        conversionState.indexInNestingLevel[conversionState.nestingLevel] = indexInNestingLevel[nestingLevel];
        conversionState.indexInNestingLevel[++conversionState.nestingLevel] = config_params::kInvalidObjectIndex;
    }

    void decreaseNestingLevel() noexcept
    {
        assert(nestingLevel > 0);
        assert(indexInNestingLevel[nestingLevel - 1] < config_params::kMaxObjectIndex);
        ++indexInNestingLevel[--nestingLevel];
    }

    void decreaseConversionNestingLevel() noexcept
    {
        assert(conversionState.nestingLevel > 0);
        --conversionState.nestingLevel;
    }  

    void increaseIndexInNestingLevel() noexcept
    {
        ++indexInNestingLevel[nestingLevel];
    }

    [[nodiscard]] bool shouldReadMe() const noexcept
    {
        if (conversionState.nestingLevel != nestingLevel)
            return false;
        for (auto i = 0; i < conversionState.nestingLevel; ++i) {
            if (indexInNestingLevel[i] != conversionState.indexInNestingLevel[i])
                return false;
        }
        return true;
    }

    [[nodiscard]] bool parseUint(std::uint64_t& result) noexcept
    {
        result = 0;
        bool parsedAtLeastOneDigit = false;
        while (readIndex < buffer.size()) {
            if (const char c = buffer[readIndex]; c >= u8'0' && c <= u8'9') {
                parsedAtLeastOneDigit = true;
                ++readIndex;
                const auto lastResult = result;
                result *= 10;
                result += c - u8'0';
                if (result < lastResult)
                    return false;
            } else {
                return parsedAtLeastOneDigit;
            }
        }
        return false;
    }

    [[nodiscard]] bool parseBool(bool& result) noexcept
    {
        const auto previousReadIndex = readIndex;
        if (readString(u8"true")) {
            result = true;
            return true;
        }

        readIndex = previousReadIndex;
        if (readString(u8"false")) {
            result = false;
            return true;
        }

        readIndex = previousReadIndex;
        return false;
    }

    [[nodiscard]] static constexpr bool isWhitespace(char8_t c) noexcept
    {
        switch (c) {
        case u8' ':
        case u8'\t':
        case u8'\n':
        case u8'\r':
            return true;
        default:
            return false;
        }
    }

    bool skipWhitespaces() noexcept
    {
        while (readIndex < buffer.size() && isWhitespace(buffer[readIndex]))
            ++readIndex;
        return true; 
    }

    [[nodiscard]] bool readChar(char8_t c) noexcept
    {
        if (readIndex < buffer.size() && buffer[readIndex] == c) {
            ++readIndex;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool readString(const char8_t* str) noexcept
    {
        while (*str && readIndex < buffer.size()) {
            if (*str++ != buffer[readIndex++])
                return false;
        }
        return *str == 0;
    }

    std::span<const char8_t> buffer;
    std::size_t readIndex{0};
    ConfigStringConversionState& conversionState;
    std::array<config_params::ObjectIndexType, config_params::kMaxNestingLevel + 1> indexInNestingLevel{};
    config_params::NestingLevelIndexType nestingLevel{0};
};
