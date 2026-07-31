#pragma once

#include <CS2/Classes/CSGOInput.h>

#include <cstddef>
#include <cstdint>

namespace aimbot::silent
{

template <typename Hooks>
[[nodiscard]] bool isAvailable(const Hooks& hooks) noexcept
{
    return hooks.createMoveHook.isInstalled();
}

template <typename Hooks>
void request(Hooks& hooks, float pitch, float yaw) noexcept
{
    hooks.silentAimPitch = pitch;
    hooks.silentAimYaw = yaw;
    hooks.silentAimActive = true;
}

template <typename Hooks>
void cancelRequest(Hooks& hooks) noexcept
{
    hooks.silentAimActive = false;
}

inline void markHasBits(void* protobufMessage, std::uint32_t bits) noexcept
{
    auto* hasBits = reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::byte*>(protobufMessage) + 0x8);
    *hasBits |= bits;
}

inline void markCsgoUserCmdHasBits(cs2::CCSGOUserCmdPB& cmd, std::uint32_t bits) noexcept
{
    cmd.hasBits |= bits;
}

inline void writeViewAngles(cs2::CMsgQAngle& viewAngles, float pitch, float yaw) noexcept
{
    markHasBits(&viewAngles, 0x7);
    viewAngles.pitch = pitch;
    viewAngles.yaw = yaw;
    viewAngles.roll = 0.0f;
}

template <typename IsReadable>
[[nodiscard]] bool writeProtobufViewAngles(cs2::CUserCmd& cmd, float pitch, float yaw, IsReadable isReadable) noexcept
{
    bool wrote = false;
    auto& csgoCmd = cmd.csgoUserCmd();

    if (isReadable(&csgoCmd, sizeof(cs2::CCSGOUserCmdPB))
        && isReadable(csgoCmd.baseCmd, sizeof(cs2::CBaseUserCmdPB))
        && isReadable(csgoCmd.baseCmd->viewAngles, sizeof(cs2::CMsgQAngle))) {
        // CCSGOUserCmdPB.basecmd is field 1. CBaseUserCmdPB.viewangles is
        // field 3 in current CS2 command protobufs.
        markCsgoUserCmdHasBits(csgoCmd, 0x1);
        auto* baseCmd = csgoCmd.baseCmd;
        markHasBits(baseCmd, 1U << 2);
        writeViewAngles(*baseCmd->viewAngles, pitch, yaw);
        wrote = true;
    }

    if (isReadable(&csgoCmd, sizeof(cs2::CCSGOUserCmdPB))
        && csgoCmd.inputHistory.currentSize > 0
        && csgoCmd.inputHistory.currentSize <= 64
        && isReadable(csgoCmd.inputHistory.rep, 0x8 + sizeof(void*) * static_cast<std::size_t>(csgoCmd.inputHistory.currentSize))) {
        for (int i = 0; i < csgoCmd.inputHistory.currentSize; ++i) {
            auto* entry = csgoCmd.inputHistory.element(i);
            if (!isReadable(entry, sizeof(cs2::CCSGOInputHistoryEntryPB)))
                continue;
            if (!isReadable(entry->viewAngles, sizeof(cs2::CMsgQAngle)))
                continue;

            markHasBits(entry, 0x1);
            writeViewAngles(*entry->viewAngles, pitch, yaw);
            wrote = true;
        }
    }

    return wrote;
}

template <typename IsReadable>
[[nodiscard]] bool applyToUserCmd(cs2::CUserCmd& cmd, float pitch, float yaw, IsReadable isReadable) noexcept
{
    return writeProtobufViewAngles(cmd, pitch, yaw, isReadable);
}

}
