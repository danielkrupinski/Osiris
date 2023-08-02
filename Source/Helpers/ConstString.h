#pragma once

#include <algorithm>
#include <string_view>

#include <MemoryAllocation/UniquePtr.h>

struct ConstString {
    explicit ConstString(std::string_view string) noexcept
        : storage{ createStorage(string) }
    {
    }

    ConstString() = default;

    [[nodiscard]] std::string_view get() const noexcept
    {
        return storage ? std::string_view{ storage.get(), storage.get_deleter().getNumberOfElements() - 1 } : "";
    }

private:
    [[nodiscard]] static UniquePtr<char[]> createStorage(std::string_view string) noexcept
    {
        if (string.empty())
            return {};

        auto storage = mem::makeUniqueForOverwrite<char[]>(string.length() + 1);
        if (storage) [[likely]] {
            std::ranges::copy(string, storage.get());
            storage[string.length()] = '\0';
        }
        return storage;
    }

    UniquePtr<char[]> storage;
};
