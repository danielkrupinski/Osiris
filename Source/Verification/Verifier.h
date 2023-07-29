#pragma once

#include <source_location>

#include <Platform/PlatformApi.h>
#include <Platform/SimpleMessageBox.h>
#include <Utils/StringBuilder.h>

template <typename Lhs, typename Rhs>
class Verifier {
public:
    Verifier(const std::source_location& location, const char* message, const char* lhsString, const char* rhsString, const Lhs& lhs, const Rhs& rhs) noexcept
        : location{ location }, message{ message }, lhsString{ lhsString }, rhsString{ rhsString }, lhs{ lhs }, rhs{ rhs }
    {
    }

    bool verifyEqual() noexcept
    {
        return verify(lhs == rhs, "==");
    }

    bool verifyNotEqual() noexcept
    {
        return verify(lhs != rhs, "!=");
    }

    bool verifyGreaterOrEqual() noexcept
    {
        return verify(lhs >= rhs, ">=");
    }

    bool verifyGreater() noexcept
    {
        return verify(lhs > rhs, ">");
    }

private:
    [[nodiscard]] bool verify(bool comparisonSucceeded, const char* comparator) noexcept
    {
        return comparisonSucceeded || comparisonFailed(comparator);
    }

    [[nodiscard]] bool comparisonFailed(const char* comparator) noexcept
    {
        showFailureMessageBox(comparator);
        PlatformApi::debugBreak();
        return false;
    }

    void showFailureMessageBox(const char* comparator) noexcept
    {
        SimpleMessageBox{}.showError("Osiris verification failure", buildFailureMessage(comparator));
    }

    [[nodiscard]] const char* buildFailureMessage(const char* comparator) noexcept
    {
        auto builder = storage.builder();
        builder.put(location.file_name(), '(', location.line(), "): ", message);
        builder.put('\n', '\n');
        builder.put(lhsString, ' ', comparator, ' ', rhsString, '\n', lhs, ' ', comparator, ' ', rhs);
        return builder.cstring();
    }

    std::source_location location;
    const char* message;
    const char* lhsString;
    const char* rhsString;
    const Lhs& lhs;
    const Rhs& rhs;
    StringBuilderStorage<1'000> storage;
};
