#pragma once

#include <cstddef>
#include <memory>

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>

class MinHook {
public:
    void init(void* base) noexcept;
    void restore() noexcept {}
    std::uintptr_t hookAt(std::size_t index, void* fun) noexcept;

private:
    void* base;
    std::unique_ptr<uintptr_t[]> originals;
};
