#pragma once

#include <limits>

#include "InWorldPanelIndex.h"

class InWorldPanelListEntry {
public:
    InWorldPanelListEntry() = default;
    explicit InWorldPanelListEntry(InWorldPanelIndex index) noexcept
        : value{static_cast<InWorldPanelIndex::ValueType>(index.value | kIsPanelActiveMask)}
    {
    }

    [[nodiscard]] bool isPanelActive() const noexcept
    {
        return (value & kIsPanelActiveMask) != 0;
    }

    void markPanelActive() noexcept
    {
        value |= kIsPanelActiveMask;
    }

    void markPanelInactive() noexcept
    {
        value &= ~kIsPanelActiveMask;
    }

    [[nodiscard]] InWorldPanelIndex indexOfNextPanelOfSameType() const noexcept
    {
        return InWorldPanelIndex{static_cast<InWorldPanelIndex::ValueType>(value & kPanelIndexMask)};
    }

private:
    static_assert(InWorldPanelIndex::kNumberOfValueBits < std::numeric_limits<InWorldPanelIndex::ValueType>::digits);

    static constexpr auto kIsPanelActiveMask{(1 << InWorldPanelIndex::kNumberOfValueBits)};
    static constexpr auto kPanelIndexMask{(1 << InWorldPanelIndex::kNumberOfValueBits) - 1};

    InWorldPanelIndex::ValueType value{InWorldPanelIndex{}.value};
};
