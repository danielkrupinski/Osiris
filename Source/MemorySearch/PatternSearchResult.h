#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <Utils/GenericPointer.h>

class PatternSearchResult {
public:
    explicit PatternSearchResult(const void* address) noexcept
        : address{ address }
    {
    }

    PatternSearchResult& add(std::ptrdiff_t offset) noexcept
    {
        if (address != nullptr)
            address = static_cast<const std::byte*>(address) + offset;
        return *this;
    }

    [[nodiscard]] GenericPointer abs() const noexcept
    {
        if (address != nullptr) {
            using OffsetType = std::int32_t;
            const auto addressOfNextInstruction = static_cast<const std::byte*>(address) + sizeof(OffsetType);
            const auto offset = derefAddress<OffsetType>();
            return addressOfNextInstruction + offset;
        }
        return {};
    }

    template <typename T>
    [[nodiscard]] T as() const noexcept
    {
        return T(address);
    }

private:
    template <typename T>
    [[nodiscard]] T derefAddress() const noexcept
    {
        T value;
        std::memcpy(&value, address, sizeof(T));
        return value;
    }

    const void* address;
};
