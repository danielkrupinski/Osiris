#pragma once

#include <cassert>
#include <cstddef>
#include <new>
#include <utility>

template <typename T>
struct ManuallyDestructible {
    ManuallyDestructible() = default;
    ManuallyDestructible(const ManuallyDestructible&) = delete;
    ManuallyDestructible(ManuallyDestructible&&) = delete;
    ManuallyDestructible& operator=(const ManuallyDestructible&) = delete;
    ManuallyDestructible& operator=(ManuallyDestructible&&) = delete;

    template <typename... Args>
    void initialize(Args&&... args) noexcept
    {
        assert(!isInitialized());
        object = new (&storage) T(std::forward<Args>(args)...);
    }

    [[nodiscard]] T* operator->() noexcept
    {
        assert(isInitialized());
        return object;
    }

    [[nodiscard]] T& operator*() noexcept
    {
        assert(isInitialized());
        return *object;
    }

    void destroy() noexcept
    {
        assert(isInitialized());
        object->~T();
        object = nullptr;
    }

    [[nodiscard]] bool isInitialized() const noexcept
    {
        return object != nullptr;
    }

private:
    T* object{nullptr};
    alignas(alignof(T)) std::byte storage[sizeof(T)]{};
};
