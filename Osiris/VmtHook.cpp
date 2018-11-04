#include "VmtHook.h"

VmtHook::VmtHook(void* base)
    : class_base(base), vmtLength(0), newVmt(nullptr), oldVmt(nullptr)
{
}
VmtHook::~VmtHook()
{
    unhook_all();

    delete[] newVmt;
}

bool VmtHook::setup(void* base /*= nullptr*/)
{
    if (base != nullptr)
        class_base = base;

    if (class_base == nullptr)
        return false;

    oldVmt = *(std::uintptr_t**)class_base;
    vmtLength = estimate_vftbl_length(oldVmt);

    if (vmtLength == 0)
        return false;

    newVmt = new std::uintptr_t[vmtLength + 1]();

    std::memcpy(&newVmt[1], oldVmt, vmtLength * sizeof(std::uintptr_t));

    try {
        auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
        newVmt[0] = oldVmt[-1];
        *(std::uintptr_t**)class_base = &newVmt[1];
    }
    catch (...) {
        delete[] newVmt;
        return false;
    }

    return true;
}

void VmtHook::unhook_index(int index)
{
    newVmt[index] = oldVmt[index];
}

void VmtHook::unhook_all()
{
    try {
        if (oldVmt != nullptr) {
            auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
            *(std::uintptr_t**)class_base = oldVmt;
            oldVmt = nullptr;
        }
    }
    catch (...) {
    }
}

std::size_t VmtHook::estimate_vftbl_length(std::uintptr_t* vftbl_start)
{
    auto len = std::size_t{};

    while (vftbl_start[len] >= 0x00010000 &&
        vftbl_start[len] < 0xFFF00000 &&
        len < 512 /* Hard coded value. Can cause problems, beware.*/) {
        len++;
    }

    return len;
}
