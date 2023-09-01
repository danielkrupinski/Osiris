#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

class SafeAddress {
public:
    explicit SafeAddress(const void* address) noexcept
        : address{ address }
    {
    }

    SafeAddress& add(std::ptrdiff_t offset) noexcept
    {
        if (address != nullptr)
            address = static_cast<const std::byte*>(address) + offset;
        return *this;
    }

    template <std::size_t N>
    SafeAddress& deref() noexcept
    {
        if constexpr (N != 0) {
            if (address != nullptr) {
                address = derefAddress<const void*>();
                return deref<N - 1>();
            }
        }
        return *this;
    }

    SafeAddress& deref() noexcept
    {
        return deref<1>();
    }

    SafeAddress& abs() noexcept
    {
        if (address != nullptr) {
            using OffsetType = std::int32_t;
            const auto addressOfNextInstruction = static_cast<const std::byte*>(address) + sizeof(OffsetType);
            const auto offset = derefAddress<OffsetType>();
            address = addressOfNextInstruction + offset;
        }
        return *this;
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
