#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct ClientState; }

class ClientStateHooks : public RefCountedHook<ClientStateHooks> {
public:
    explicit ClientStateHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::ClientState* clientState)
        : hookImpl{ vmtLengthCalculator }, clientState{ clientState }
    {
    }

    static void FASTCALL_CONV packetEnd(FASTCALL_THIS(csgo::ClientState* thisptr)) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(clientState));
        originalPacketEnd = hookImpl.hook(WIN32_LINUX(6, 7), &packetEnd);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(clientState));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(clientState));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::ClientState* clientState;

    void (THISCALL_CONV* originalPacketEnd)(csgo::ClientState* thisptr);
};
