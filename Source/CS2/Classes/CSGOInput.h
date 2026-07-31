#pragma once

#include <cstddef>
#include <cstdint>

namespace cs2
{

struct CUserCmd;

struct CCSGOInput {
    using CreateMove = bool(CCSGOInput*, int slot, CUserCmd* cmd);
};

struct CMsgQAngle {
    std::byte pad[0x18];
    float pitch;
    float yaw;
    float roll;
    std::byte alignmentPadding[4];
};

template <typename T>
struct RepeatedPtrField {
    void* arena;
    int currentSize;
    int totalSize;
    void* rep;

    [[nodiscard]] T* element(int index) noexcept
    {
        auto** elements = reinterpret_cast<T**>(reinterpret_cast<std::byte*>(rep) + 0x8);
        return elements[index];
    }
};

struct CCSGOInputHistoryEntryPB {
    std::byte pad[0x18];
    CMsgQAngle* viewAngles;
};

struct CBaseUserCmdPB {
    std::byte pad[0x40];
    CMsgQAngle* viewAngles;
};

struct CCSGOUserCmdPB {
    std::uint32_t hasBits;
    std::uint64_t cachedSize;
    RepeatedPtrField<CCSGOInputHistoryEntryPB> inputHistory;
    CBaseUserCmdPB* baseCmd;
    bool leftHandDesired;
    std::byte pad[3];
    std::int32_t attack3StartHistoryIndex;
    std::int32_t attack1StartHistoryIndex;
    std::int32_t attack2StartHistoryIndex;
};

struct CUserCmd {
    std::byte pad[0x18];
    CCSGOUserCmdPB protobuf;
    std::byte buttonsAndTail[0x40];

    [[nodiscard]] CBaseUserCmdPB* baseCmdAt(std::ptrdiff_t offset) noexcept
    {
        return *reinterpret_cast<CBaseUserCmdPB**>(reinterpret_cast<std::byte*>(this) + offset);
    }

    [[nodiscard]] CCSGOUserCmdPB& csgoUserCmd() noexcept
    {
        return protobuf;
    }
};

static_assert(offsetof(CMsgQAngle, pitch) == 0x18);
static_assert(sizeof(CMsgQAngle) == 0x28);
static_assert(offsetof(CCSGOInputHistoryEntryPB, viewAngles) == 0x18);
static_assert(offsetof(CBaseUserCmdPB, viewAngles) == 0x40);
static_assert(offsetof(CCSGOUserCmdPB, inputHistory) == 0x10);
static_assert(offsetof(CCSGOUserCmdPB, baseCmd) == 0x28);
static_assert(sizeof(CCSGOUserCmdPB) == 0x40);
static_assert(offsetof(CUserCmd, protobuf) == 0x18);
static_assert(sizeof(CUserCmd) == 0x98);

}
