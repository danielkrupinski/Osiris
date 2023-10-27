#pragma once

#include <cassert>
#include <utility>

template <typename T>
struct LazyInitialized {
    LazyInitialized() noexcept
        : empty{}
    {
    }

    LazyInitialized(const LazyInitialized&) = delete;
    LazyInitialized(LazyInitialized&&) = delete;
    LazyInitialized& operator=(const LazyInitialized&) = delete;
    LazyInitialized& operator=(LazyInitialized&&) = delete;

    template <typename... Args>
    void init(Args&&... args) noexcept
    {
        assert(!initialized);
        new (&value) T{ std::forward<Args>(args)... };
        initialized = true;
    }

    [[nodiscard]] T* operator->() noexcept
    {
        assert(initialized);
        return &value;
    }

    [[nodiscard]] T& operator*() noexcept
    {
        assert(initialized);
        return value;
    }

    ~LazyInitialized() noexcept
    {
        if (initialized)
            value.~T();
    }

private:
    struct Empty {};

    union {
        Empty empty;
        T value;
    };
    bool initialized = false;
};
