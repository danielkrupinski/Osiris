#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct ClientPOD;
    enum class FrameStage;
    enum class UserMessageType;
}

class ClientHooks : public RefCountedHook<ClientHooks> {
public:
    ClientHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::ClientPOD* client)
        : hookImpl{ vmtLengthCalculator }, client{ client }
    {
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
    void install() noexcept
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(client));
        originalFrameStageNotify = hookImpl.hook(37, &frameStageNotify);
        originalDispatchUserMessage = hookImpl.hook(38, &dispatchUserMessage);
    }

    void uninstall() noexcept
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(client));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(client));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::ClientPOD* client;

    void (THISCALL_CONV* originalFrameStageNotify)(csgo::ClientPOD* thisptr, csgo::FrameStage stage);
    bool (THISCALL_CONV* originalDispatchUserMessage)(csgo::ClientPOD* thisptr, csgo::UserMessageType type, int passthroughFlags, int size, const void* data);
};
