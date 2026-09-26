#pragma once

#include <cassert>

#include <MemorySearch/BytePattern.h>
#include <Utils/StringBuilder.h>
#include <Platform/SimpleMessageBox.h>

class PatternNotFoundLogger {
public:
    PatternNotFoundLogger() = default;
    PatternNotFoundLogger(const PatternNotFoundLogger&) = delete;
    PatternNotFoundLogger& operator=(const PatternNotFoundLogger&) = delete;
    PatternNotFoundLogger(PatternNotFoundLogger&&) = delete;
    PatternNotFoundLogger& operator=(PatternNotFoundLogger&&) = delete;

    void onPatternNotFound(BytePattern pattern) noexcept
    {
        if (bufferEmpty) {
            builder.put("Failed to find patterns:\n");
            bufferEmpty = false;
        } else {
            builder.put(" | ");
        }

        bool printedFirst = false;
        const auto wildcardChar{pattern.getWildcardChar()};
        for (const auto byte : pattern.raw()) {
            if (printedFirst)
                builder.put(' ');
            if (byte != wildcardChar) {
                if ((byte & 0xF0) == 0)
                    builder.put('0');
                builder.putHex(static_cast<unsigned char>(byte));
            } else {
                builder.put(byte);
            }

            printedFirst = true;
        }
    }

    void finish() noexcept
    {
        if (!bufferEmpty)
            SimpleMessageBox{}.showWarning("Osiris", builder.cstring());
    }

    [[nodiscard]] bool isLogEmpty() const noexcept
    {
        return bufferEmpty;
    }

private:
    StringBuilderStorage<2000> storage;
    StringBuilder builder{storage.builder()};
    bool bufferEmpty = true;
};
