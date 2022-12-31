#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

#include "PatternFinder.h"

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

namespace helpers
{

SafeAddress PatternFinder::operator()(std::string_view pattern) const noexcept
{
    if (const auto found = ::PatternFinder::operator()(pattern))
        return SafeAddress{ std::uintptr_t(found) };

    assert(false && "Pattern needs to be updated!");
#if IS_WIN32()
    MessageBoxA(nullptr, ("Failed to find pattern:\n" + patternToString(pattern)).c_str(), "Osiris", MB_OK | MB_ICONWARNING);
#endif
    return SafeAddress{ 0 };
}

SafeAddress PatternFinder::operator()(std::string_view pattern, OffsetHint offsetHint) const noexcept
{
    if (const auto found = ::PatternFinder::operator()(pattern, offsetHint))
        return SafeAddress{ std::uintptr_t(found) };

    assert(false && "Pattern needs to be updated!");
#if IS_WIN32()
    MessageBoxA(nullptr, ("Failed to find pattern:\n" + patternToString(pattern)).c_str(), "Osiris", MB_OK | MB_ICONWARNING);
#endif
    return SafeAddress{ 0 };
}

}
