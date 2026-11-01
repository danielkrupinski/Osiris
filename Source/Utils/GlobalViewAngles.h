#pragma once

#include <cstddef>

#include <CS2/Offsets/ClientDllOffsets.h>
#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()
#include <Windows.h>
#endif

namespace utils
{

// Resolves the local player's view angles through the CCSGOInput singleton.
// client.dll + kCSGOInput stores the pointer to the factory-created input
// object; the slot-0 angles live at kViewAnglesInInput inside that object
// (CS2 1.41.9.2). The object pointer is validated (committed memory, readable
// vtable, angles inside the same region) so that a stale offset degrades to
// "no angles available" instead of writing floats into unrelated client.dll
// data — the failure mode that crashed the game when a raw module+offset
// address went stale after a game update.
[[nodiscard]] inline float* globalViewAngles() noexcept
{
#if IS_WIN64()
    const auto clientDll = GetModuleHandleA("client.dll");
    if (!clientDll)
        return nullptr;

    const auto isCommitted = [](const void* address, std::size_t size, void** regionEnd) noexcept {
        MEMORY_BASIC_INFORMATION info{};
        if (!VirtualQuery(address, &info, sizeof(info)))
            return false;
        if (info.State != MEM_COMMIT || (info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0)
            return false;
        *regionEnd = static_cast<std::byte*>(info.BaseAddress) + info.RegionSize;
        const auto start = static_cast<const std::byte*>(address);
        return start + size > start && start + size <= static_cast<std::byte*>(*regionEnd);
    };

    auto* const input = *reinterpret_cast<std::byte**>(
        reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kCSGOInput);
    if (!input)
        return nullptr;

    void* regionEnd{};
    if (!isCommitted(input, sizeof(void*), &regionEnd))
        return nullptr;
    // The input object starts with its vtable pointer; checking that it is
    // readable rejects garbage left at the global before initialization.
    if (!isCommitted(*reinterpret_cast<void**>(input), sizeof(void*), &regionEnd))
        return nullptr;

    auto* const angles = reinterpret_cast<float*>(
        input + cs2::client_dll_offsets::kViewAnglesInInput);
    if (!isCommitted(angles, 3 * sizeof(float), &regionEnd))
        return nullptr;

    return angles;
#else
    return nullptr;
#endif
}

}
