#pragma once

#include <Endpoints.h>
#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo
{
    struct ClientPOD;
    enum class FrameStage;
    enum class UserMessageType;
}

class ClientHooks {
public:
    void install(csgo::ClientPOD* client)
    {
        hookImpl.init(client);
        originalFrameStageNotify = reinterpret_cast<decltype(originalFrameStageNotify)>(hookImpl.hookAt(37, &frameStageNotify));
        originalDispatchUserMessage = reinterpret_cast<decltype(originalDispatchUserMessage)>(hookImpl.hookAt(38, &dispatchUserMessage));
    }

    void uninstall()
    {
        hookImpl.restore();
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
