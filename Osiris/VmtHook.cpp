#include "VmtHook.h"

VmtHook::~VmtHook()
{
    unhook_all();

    delete[] newVmt;
}

bool VmtHook::setup(void* base /*= nullptr*/)
{
    if (!base)
        return false;

    class_base = base;

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

std::size_t VmtHook::estimate_vftbl_length(std::uintptr_t* vftbl)
{
    std::size_t length{ 0 };

    while (vftbl[length] >= 0x10000 && vftbl[length] < 0xFFF00000 && length < 512)
        length++;

    return length;
}
