#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct ClientPOD;
    enum class FrameStage;
    enum class UserMessageType;
}

class ClientHooks {
public:
    explicit ClientHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::ClientPOD* client)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(client));
        originalFrameStageNotify = reinterpret_cast<decltype(originalFrameStageNotify)>(hookImpl.hook(37, std::uintptr_t(&frameStageNotify)));
        originalDispatchUserMessage = reinterpret_cast<decltype(originalDispatchUserMessage)>(hookImpl.hook(38, std::uintptr_t(&dispatchUserMessage)));
    }

    void uninstall(csgo::ClientPOD* client)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(client));
    }

    [[nodiscard]] auto getOriginalFrameStageNotify() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalFrameStageNotify };
    }

    [[nodiscard]] auto getOriginalDispatchUserMessage() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalDispatchUserMessage };
    }

    static void FASTCALL_CONV frameStageNotify(FASTCALL_THIS(csgo::ClientPOD* thisptr), csgo::FrameStage stage) noexcept;
    static bool FASTCALL_CONV dispatchUserMessage(FASTCALL_THIS(csgo::ClientPOD* thisptr), csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept;

private:
    HookType hookImpl;

    void (THISCALL_CONV* originalFrameStageNotify)(csgo::ClientPOD* thisptr, csgo::FrameStage stage);
    bool (THISCALL_CONV* originalDispatchUserMessage)(csgo::ClientPOD* thisptr, csgo::UserMessageType type, int passthroughFlags, int size, const void* data);
};
