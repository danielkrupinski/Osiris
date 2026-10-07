#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

#include <MemorySearch/PatternSearchResult.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <Utils/MemorySection.h>
#include <Utils/GenericPointer.h>

#include "BytePattern.h"
#include "BytePatternView.h"
#include "CodePatternOperation.h"
#include "HybridPatternFinder.h"
#include "PatternPoolView.h"
#include "PatternSearchResults.h"
#include "PatternSearchResultsView.h"
#include "PatternStringWildcard.h"

template <typename NotFoundHandler>
struct PatternFinder {
public:
    explicit PatternFinder(std::span<const std::byte> bytes) noexcept
        : bytes{bytes}
    {
    }

    [[nodiscard]] auto findPatterns(const auto& patterns) const noexcept
    {
        PatternSearchResults<std::remove_reference_t<decltype(patterns)>> results;
        findPatterns(patterns.getView(), results);
        return results;
    }

    [[NOINLINE]] void findPatterns(PatternPoolView patterns, auto& results) const noexcept
    {
        NotFoundHandler notFoundHandler;
        const auto resultView = results.getView();
        patterns.forEach([patternIndex = std::size_t{0}, resultView, this, &notFoundHandler](BytePattern pattern, std::uint8_t offset, CodePatternOperation operation) mutable {
            auto result = operator()(pattern, notFoundHandler);
            result.add(offset);

            std::array<std::byte, 8> resultToStore{};
            if (operation == CodePatternOperation::None) {
                resultToStore = result.get();
            } else if (operation == CodePatternOperation::Abs4 || operation == CodePatternOperation::Abs5) {
                resultToStore = result.abs2(operation == CodePatternOperation::Abs4 ? 4 : 5);
            } else if (operation == CodePatternOperation::Read) {
                resultToStore = result.read();
            }
            resultView.store(patternIndex, resultToStore);
            ++patternIndex;
        });

        notFoundHandler.finish();
        results.setValid(notFoundHandler.isLogEmpty());
        assert(notFoundHandler.isLogEmpty() && "Patterns needs to be updated!");
    }

    template <std::size_t PatternLength>
    [[nodiscard]] PatternSearchResult matchPatternAtAddress(GenericPointer address, BytePatternView<PatternLength> patternView) const noexcept
    {
        return matchPatternAtAddress(address, BytePattern{{patternView.data(), PatternLength}, kPatternStringWildcard});
    }

    [[nodiscard]] PatternSearchResult matchPatternAtAddress(GenericPointer address, BytePattern pattern) const noexcept
    {
        if (matchesPatternAtAddress(address, pattern))
            return makeResult(address.as<const std::byte*>(), pattern.length());
        return makeResult(nullptr, pattern.length());
    }

private:
    [[nodiscard]] [[NOINLINE]] PatternSearchResult operator()(BytePattern pattern, NotFoundHandler& notFoundHandler) const noexcept
    {
        auto patternFinder = HybridPatternFinder{bytes, pattern};
        const auto found = patternFinder.findNextOccurrence();
        const auto duplicate = found && patternFinder.findNextOccurrence() != nullptr;
        if (duplicate)
            notFoundHandler.onPatternNotUnique(pattern);
        assert(!duplicate && "Pattern should be unique!");
        if (!found)
            notFoundHandler.onPatternNotFound(pattern);
        return makeResult(found, pattern.length());
    }

    [[nodiscard]] bool matchesPatternAtAddress(GenericPointer address, BytePattern pattern) const noexcept
    {
        if (MemorySection{bytes}.contains(address.as<std::uintptr_t>(), pattern.length()))
            return pattern.matches(std::span{address.as<const std::byte*>(), pattern.length()});
        return false;
    }

    [[nodiscard]] PatternSearchResult makeResult(const std::byte* address, std::size_t patternLength) const noexcept
    {
        if (address)
            return PatternSearchResult{bytes.data(), static_cast<std::size_t>(address - bytes.data()), std::span{address, patternLength}};
        return {};
    }

    std::span<const std::byte> bytes;
};
