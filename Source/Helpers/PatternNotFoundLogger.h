#pragma once

#include <MemorySearch/BytePattern.h>
#include <Utils/StringBuilder.h>
#include <Platform/SimpleMessageBox.h>

struct PatternNotFoundLogger {
    void operator()(BytePattern pattern, std::span<const std::byte> bytes) const
    {
        StringBuilderStorage<500> storage;
        auto builder = storage.builder();

        assert(false && "Pattern needs to be updated!");

        builder.put("Failed to find pattern ");

        bool printedFirst = false;
        for (const auto byte : pattern.get()) {
            if (printedFirst)
                builder.put(' ');

            if (byte != BytePattern::wildcardChar) {
                if ((byte & 0xF0) == 0)
                    builder.put('0');
                builder.putHex(static_cast<unsigned char>(byte));
            } else {
                builder.put(BytePattern::wildcardChar);
            }

            printedFirst = true;
        }

        builder.put('\n');

        messageBox.showError("Error", builder.cstring());
    }

private:
    SimpleMessageBox messageBox;
};
