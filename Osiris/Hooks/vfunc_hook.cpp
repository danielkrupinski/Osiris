#include "vfunc_hook.hpp"

vfunc_hook::vfunc_hook()
    : class_base(nullptr), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
vfunc_hook::vfunc_hook(void* base)
    : class_base(base), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
vfunc_hook::~vfunc_hook()
{
    unhook_all();

    delete[] new_vftbl;
}

bool vfunc_hook::setup(void* base /*= nullptr*/)
{
    if(base != nullptr)
        class_base = base;

    if(class_base == nullptr)
        return false;

    old_vftbl = *(std::uintptr_t**)class_base;
    vftbl_len = estimate_vftbl_length(old_vftbl);

    if(vftbl_len == 0)
        return false;

    new_vftbl = new std::uintptr_t[vftbl_len]();

    std::memcpy(new_vftbl, old_vftbl, vftbl_len * sizeof(std::uintptr_t));

    try {
        auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
        *(std::uintptr_t**)class_base = new_vftbl;
    } catch(...) {
        delete[] new_vftbl;
        return false;
    }

    return true;
}
std::size_t vfunc_hook::estimate_vftbl_length(std::uintptr_t* vftbl_start)
{
    auto len = std::size_t{};

    while(vftbl_start[len] >= 0x00010000 &&
        vftbl_start[len] <  0xFFF00000 &&
        len < 512 /* Hard coded value. Can cause problems, beware.*/) {
        len++;
    }

    return len;
}