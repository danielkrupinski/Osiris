#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

class SafeAddress {
public:
    explicit SafeAddress(std::uintptr_t address)
        : address{ address }
    {
    }

    SafeAddress& add(std::ptrdiff_t offset) noexcept
    {
        if (address != 0)
            address += offset;
        return *this;
    }

    template <std::size_t N>
    SafeAddress& deref() noexcept
    {
        if constexpr (N != 0) {
            if (address != 0) {
                address = *reinterpret_cast<std::uintptr_t*>(address);
                return deref<N - 1>();
            }
        }
        return *this;
    }

    SafeAddress& deref() noexcept
    {
        return deref<1>();
    }

    SafeAddress& relativeToAbsolute() noexcept
    {
        if (address != 0) {
            using OffsetType = std::int32_t;
            const auto addressOfNextInstruction = address + sizeof(OffsetType);
            address = addressOfNextInstruction + *reinterpret_cast<OffsetType*>(address);
        }
        return *this;
    }

    [[nodiscard]] std::uintptr_t get() const noexcept
    {
        return address;
    }

    template <typename T>
    [[nodiscard]] T as() const noexcept
    {
        static_assert(std::is_pointer_v<T>, "T must be a pointer type!");
        return reinterpret_cast<T>(address);
    }

private:
    std::uintptr_t address;
};
