#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

#include <MemorySearch/BytePattern.h>
#include "PatternNotFoundHandler.h"
#include <Platform/Macros/IsPlatform.h>

[[nodiscard]] static std::string patternToString(std::string_view pattern)
{
    std::ostringstream os;

    bool printedFirst = false;
    for (auto byte : pattern) {
        if (printedFirst)
            os << ' ';

        if (byte != '?')
            os << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<unsigned>(static_cast<unsigned char>(byte));
        else
            os << '?';

        printedFirst = true;
    }

    return os.str();
}

void PatternNotFoundHandler::operator()(BytePattern pattern) const
{
    assert(false && "Pattern needs to be updated!");
#if IS_WIN32()
    MessageBoxA(nullptr, ("Failed to find pattern:\n" + patternToString(pattern.get())).c_str(), "Osiris", MB_OK | MB_ICONWARNING);
#endif
}
