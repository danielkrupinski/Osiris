#include <Windows.h>

#include "VmtHook.h"

static auto calculateVmtLength(uintptr_t* vmt) noexcept
{
    std::size_t length = 0;
    MEMORY_BASIC_INFORMATION memoryInfo;
    while (VirtualQuery(LPCVOID(vmt[length]), &memoryInfo, sizeof(memoryInfo)) && memoryInfo.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
        length++;
    return length;
}

void VmtHook::init(void* base) noexcept
{
    this->base = base;
    auto vmt = *reinterpret_cast<uintptr_t**>(base);
    length = calculateVmtLength(vmt);
    oldVmt = std::make_unique<uintptr_t[]>(length);
    std::copy(vmt, vmt + length, oldVmt.get());
}

void VmtHook::restore() const noexcept
{
    auto vmt = *reinterpret_cast<uintptr_t**>(base);
    if (DWORD oldProtection; VirtualProtect(vmt, length, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        std::copy(oldVmt.get(), oldVmt.get() + length, vmt);
        VirtualProtect(vmt, length, oldProtection, nullptr);
    }
}

void VmtHook::hookAt(std::size_t index, void* fun) const noexcept
{
    auto address = *reinterpret_cast<uintptr_t**>(base) + index;
    if (DWORD oldProtection; VirtualProtect(address, sizeof(address), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *address = uintptr_t(fun);
        VirtualProtect(address, sizeof(address), oldProtection, nullptr);
    }
}
