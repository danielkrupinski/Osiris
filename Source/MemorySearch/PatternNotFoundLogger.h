#pragma once

#include <cassert>

#include <MemorySearch/BytePattern.h>
#include <Utils/StringBuilder.h>
#include <Platform/SimpleMessageBox.h>

struct PatternNotFoundLogger {
    static void onPatternNotFound(BytePattern pattern) noexcept
    {
        StringBuilderStorage<500> storage;
        auto builder = storage.builder();

        assert(false && "Pattern needs to be updated!");

        builder.put("Failed to find pattern ");

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

        builder.put('\n');

        SimpleMessageBox{}.showWarning("Osiris", builder.cstring());
    }
};
