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

    newVmt = new std::uintptr_t[vmtLength];

    std::memcpy(newVmt, oldVmt, vmtLength * sizeof(std::uintptr_t));
    *(std::uintptr_t**)class_base = newVmt;
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
