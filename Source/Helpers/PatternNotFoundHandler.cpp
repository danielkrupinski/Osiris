#include <cassert>
#include <string>
#include <string_view>

#include <MemorySearch/BytePattern.h>
#include "PatternNotFoundHandler.h"
#include <Platform/Macros/IsPlatform.h>
#include <Platform/SimpleMessageBox.h>

#include <Utils/StringBuilder.h>

struct PatternToString {
    explicit PatternToString(BytePattern pattern)
        : pattern{ pattern }
    {
    }

    void operator()(StringBuilder& builder) const noexcept
    {
        bool printedFirst = false;
        for (auto byte : pattern.get()) {
            if (printedFirst)
                builder.put(' ');

            if (byte != '?')
                builder.putHex(static_cast<unsigned char>(byte));
            else
                builder.put('?');

            printedFirst = true;
        }
    }

private:
    BytePattern pattern;
};

void PatternNotFoundHandler::operator()(BytePattern pattern) const
{
    assert(false && "Pattern needs to be updated!");
    SimpleMessageBox{}.showWarning("Osiris", StringBuilderStorage<300>{}.builder().put("Failed to find pattern:\n", PatternToString{ pattern }).cstring());
}
