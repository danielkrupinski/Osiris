#pragma once

#define NOMINMAX
#include <Windows.h>

#include <cstdint>
#include <stdexcept>

namespace detail
{
    class protect_guard
    {
    public:
        protect_guard(void* base, size_t len, std::uint32_t flags)
        {
            _base = base;
            _length = len;
            if (!VirtualProtect(base, len, flags, (PDWORD)&_old))
                throw std::runtime_error("Failed to protect region.");
        }
        ~protect_guard()
        {
            VirtualProtect(_base, _length, _old, (PDWORD)&_old);
        }

    private:
        void*         _base;
        size_t        _length;
        std::uint32_t _old;
    };
}

class VmtHook {
public:
    ~VmtHook();

    bool setup(void* class_base = nullptr);

    template<typename T>
    void hookAt(int index, T fun)
    {
        if (static_cast<std::size_t>(index) < vmtLength)
            newVmt[index] = reinterpret_cast<std::uintptr_t>(fun);
    }

    void unhook_index(int);
    void unhook_all();

    template<typename T>
    constexpr auto getOriginal(int index) noexcept
    {
        return reinterpret_cast<T>(oldVmt[index]);
    }

private:
    static inline std::size_t estimate_vftbl_length(std::uintptr_t* vftbl_start);

    void*           class_base{ nullptr };
    std::size_t     vmtLength{ 0 };
    std::uintptr_t* newVmt{ nullptr };
    std::uintptr_t* oldVmt{ nullptr };
};
