#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <span>

#include "ConfigParams.h"
#include "ConfigStringConversionState.h"

class ConfigToString {
public:
    ConfigToString(std::span<char8_t> buffer, ConfigStringConversionState& conversionState) noexcept
        : buffer{buffer}
        , conversionState{conversionState}
    {
    }

    void beginRoot() noexcept
    {
        assert(nestingLevel == 0 && "beginRoot() called on invalid nesting level");
        assert(indexInNestingLevel[0] == 0 && "can not beginRoot() after endRoot()");

        if (shouldWriteMe() && writeChar(u8'{'))
            increaseConversionNestingLevel();
        increaseNestingLevel();
    }

    [[nodiscard]] std::size_t endRoot() noexcept
    {
        assert(nestingLevel == 1 && "endRoot() called on invalid nesting level");
        assert(indexInNestingLevel[0] == 0 && "endRoot() has been already called");

        if (shouldWriteMe() && writeChar(u8'}'))
            decreaseConversionNestingLevel();
        decreaseNestingLevel();
        return writeIndex;
    }

    void beginObject(const char8_t* key) noexcept
    {
        if (shouldWriteMe()) {
            const auto previousWriteIndex = writeIndex;
            if (writeCommaAfterPreviousElement() && writeKey(key) && writeChar(u8'{'))
                increaseConversionNestingLevel();
            else
                writeIndex = previousWriteIndex;
        }
        increaseNestingLevel();
    }

    void endObject() noexcept
    {
        if (shouldWriteMe() && writeChar(u8'}'))
            decreaseConversionNestingLevel();
        decreaseNestingLevel();
    }

    void boolean(const char8_t* key, auto&& /* valueSetter */, auto&& valueGetter) noexcept
    {
        writeBool(key, valueGetter());
    }

    void uint(const char8_t* key, auto&& /* valueSetter */, auto&& valueGetter) noexcept
    {
        writeUint(key, valueGetter());
    }

private:
    void writeBool(const char8_t* key, bool value) noexcept
    {
        if (shouldWriteMe()) {
            const auto previousWriteIndex = writeIndex;
            if (writeCommaAfterPreviousElement() && writeKey(key) && writeBool(value))
                increaseConversionIndexInNestingLevel();
            else
                writeIndex = previousWriteIndex;
        }
        increaseIndexInNestingLevel();
    }

    void writeUint(const char8_t* key, std::uint64_t value) noexcept
    {
        if (shouldWriteMe()) {
            const auto previousWriteIndex = writeIndex;
            if (writeCommaAfterPreviousElement() && writeKey(key) && writeUint(value))
                increaseConversionIndexInNestingLevel();
            else
                writeIndex = previousWriteIndex;
        }
        increaseIndexInNestingLevel();
    }

    [[nodiscard]] bool writeKey(const char8_t* key) noexcept
    {
        return writeChar(u8'"') && writeString(key) && writeString(u8"\":");
    }

    [[nodiscard]] bool writeCommaAfterPreviousElement() noexcept
    {
        if (conversionState.indexInNestingLevel[conversionState.nestingLevel] != 0)
            return writeChar(u8',');
        return true;
    }

    void increaseNestingLevel() noexcept
    {
        assert(nestingLevel < config_params::kMaxNestingLevel);
        indexInNestingLevel[++nestingLevel] = 0;
    }

    void increaseConversionNestingLevel()const noexcept
    {
        assert(conversionState.nestingLevel < config_params::kMaxNestingLevel);
        conversionState.indexInNestingLevel[++conversionState.nestingLevel] = 0;
    }

    void decreaseNestingLevel() noexcept
    {
        assert(nestingLevel > 0);
        assert(indexInNestingLevel[nestingLevel - 1] < config_params::kMaxObjectIndex);
        ++indexInNestingLevel[--nestingLevel];
    }

    void decreaseConversionNestingLevel() const noexcept
    {
        assert(conversionState.nestingLevel > 0);
        assert(conversionState.indexInNestingLevel[conversionState.nestingLevel - 1] < config_params::kMaxObjectIndex);
        ++conversionState.indexInNestingLevel[--conversionState.nestingLevel];
    }
    
    void increaseIndexInNestingLevel() noexcept
    {
        assert(indexInNestingLevel[nestingLevel] < config_params::kMaxObjectIndex);
        ++indexInNestingLevel[nestingLevel];
    }

    void increaseConversionIndexInNestingLevel() const noexcept
    {
        assert(conversionState.indexInNestingLevel[conversionState.nestingLevel] < config_params::kMaxObjectIndex);
        ++conversionState.indexInNestingLevel[conversionState.nestingLevel];
    }

    [[nodiscard]] bool shouldWriteMe() const noexcept
    {
        if (conversionState.nestingLevel != nestingLevel)
            return false;
        for (config_params::NestingLevelIndexType i = 0; i <= nestingLevel; ++i) {
            if (indexInNestingLevel[i] != conversionState.indexInNestingLevel[i])
                return false;
        }
        return true;
    }

    [[nodiscard]] bool writeUint(std::uint64_t value) noexcept
    {
        const auto beginIndex = writeIndex;
        do {
            if (!writeChar(u8"0123456789"[value % 10])) {
                writeIndex = beginIndex;
                return false;
            }
            value /= 10;
        } while (value != 0);

        std::reverse(buffer.data() + beginIndex, buffer.data() + writeIndex);
        return true;
    }

    [[nodiscard]] bool writeBool(bool value) noexcept
    {
        return writeString(value ? u8"true" : u8"false");
    }

    [[nodiscard]] bool writeChar(char8_t c) noexcept
    {
        if (writeIndex < buffer.size()) {
            buffer[writeIndex++] = c;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool writeString(std::u8string_view str) noexcept
    {
        if (writeIndex < buffer.size() && buffer.size() - writeIndex >= str.length()) {
            std::ranges::copy(str, &buffer[writeIndex]);
            writeIndex += str.length();
            return true;
        }
        return false;
    }

    std::span<char8_t> buffer;
    ConfigStringConversionState& conversionState;
    std::size_t writeIndex{0};
    config_params::NestingLevelIndexType nestingLevel{0};
    std::array<config_params::ObjectIndexType, config_params::kMaxNestingLevel + 1> indexInNestingLevel{};
};
