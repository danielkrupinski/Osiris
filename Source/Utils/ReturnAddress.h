#pragma once

#include <cstdint>

struct ReturnAddress {
    ReturnAddress() = default;

    explicit ReturnAddress(const void* returnAddress) noexcept
        : returnAddress{ returnAddress }
    {
    }

    bool operator==(const ReturnAddress&) const = default;

private:
    const void* returnAddress;
};
