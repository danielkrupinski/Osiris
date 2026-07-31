#pragma once

#include <cstddef>
#include <cstdint>

#include <CS2/Classes/CSGOInput.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/GenericFunctionPointer.h>
#include <Vmt/VmtLengthCalculator.h>
#include <Vmt/VmtSwapper.h>

// Forward-declared in EntryPoints.
bool CreateMoveHook_onCreateMove(cs2::CCSGOInput* thisptr, int slot, cs2::CUserCmd* cmd) noexcept;

class CreateMoveHook {
public:
    CreateMoveHook(const VmtLengthCalculator& vmtLengthCalculator) noexcept
        : vmtLengthCalculator{vmtLengthCalculator}
    {
    }

    [[nodiscard]] cs2::CCSGOInput::CreateMove* originalCreateMove() const noexcept
    {
        return original;
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hook.wasEverInstalled();
    }

    void uninstall() const noexcept
    {
        // VMT is on a singleton — not safe to fully uninstall.
    }

    void install(cs2::CCSGOInput* csgoInput) noexcept
    {
        if (!csgoInput)
            return;
        auto** vmt = reinterpret_cast<std::uintptr_t**>(csgoInput);
        if (!vmt || !*vmt)
            return;

        // CCSGOInput::CreateMove vtable index for CS2 build 14171. Verified
        // against the live vtable: slot 25 is called once per input frame with
        // (this, split-screen slot, CUserCmd*). Slot 13 only releases an
        // internal reference and therefore made the hook appear installed
        // without ever consuming a silent-aim request.
        constexpr auto kCreateMoveVtableIndex = 25;
        if (static_cast<std::size_t>(vmtLengthCalculator(*vmt)) <= kCreateMoveVtableIndex)
            return;

        if (hook.install(vmtLengthCalculator, *vmt)) {
            original = (cs2::CCSGOInput::CreateMove*)
                hook.hook(kCreateMoveVtableIndex,
                    (GenericFunctionPointer)&CreateMoveHook_onCreateMove);
        }
    }

    void install(cs2::CCSGOInput** csgoInput) noexcept
    {
        if (!csgoInput)
            return;
        install(*csgoInput);
    }

private:
    VmtLengthCalculator vmtLengthCalculator;
    VmtSwapper hook;
    cs2::CCSGOInput::CreateMove* original{nullptr};
};
