#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct ClientState; }

class ClientStateHooks {
public:
    explicit ClientStateHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::ClientState* clientState)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(clientState));
        originalPacketEnd = reinterpret_cast<decltype(originalPacketEnd)>(hookImpl.hook(WIN32_LINUX(6, 7), std::uintptr_t(&packetEnd)));
    }

    void uninstall(csgo::ClientState* clientState)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(clientState));
    }

    static void FASTCALL_CONV packetEnd(FASTCALL_THIS(csgo::ClientState* thisptr)) noexcept;

private:
    HookType hookImpl;

    void (THISCALL_CONV* originalPacketEnd)(csgo::ClientState* thisptr);
};
