#pragma once

#define NOMINMAX
#include <Windows.h>

#include <cstdint>
#include <stdexcept>

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
