#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <Platform/Macros/CallingConventions.h>

class VmtHook {
public:
    void init(void* base) noexcept;
    void restore() const noexcept;
    std::uintptr_t hookAt(std::size_t index, void* fun) const noexcept;

private:
    void* base;
    std::size_t length;
    std::unique_ptr<std::uintptr_t[]> oldVmt;
};
