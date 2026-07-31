#pragma once

#include <cstddef>
#include <cstdint>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Offsets/ClientDllOffsets.h>
#include <CS2/Classes/Vector.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/IsPlatform.h>

#include "BHopConfigVariables.h"
#include "BHopState.h"

#if IS_WIN64()
#include <Windows.h>
#endif

// CS2 BunnyHop + Auto-Strafe.
//
// Reads the local player's ground state, velocity, and movement services
// to inject perfectly-timed jumps and optimize air-strafe direction.
// Humanization adds random timing jitter and occasional missed jumps to
// avoid VACnet's tick-perfect jump detection.
//
// Schema offsets verified with a2x/cs2-dumper build 14171.
template <typename HookContext>
class BHop {
public:
    explicit BHop(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , enabled{GET_CONFIG_VAR(bhop_vars::Enabled)}
    {
    }

    // Call once per frame (in the entity loop, for the local player).
    void run() noexcept
    {
        if (!enabled) {
            if (const auto pawn = getLocalPlayerPawn())
                clearAutoStrafe(reinterpret_cast<std::byte*>(pawn));
            releaseJump();
            state().wasOnGround = false;
            state().shouldJump = false;
            return;
        }

        const auto pawn = getLocalPlayerPawn();
        if (!pawn || !isReadableMemory(pawn, kPawnReadableSize)) {
            releaseJump();
            releaseInjectedSideButton();
            state().wasOnGround = false;
            state().shouldJump = false;
            return;
        }
        auto* bytes = reinterpret_cast<std::byte*>(pawn);

        const bool jumpHeld = isKeyHeld(VK_SPACE);
        if (!jumpHeld) {
            if (state().shouldJump)
                releaseJump(bytes);
            else
                releaseJump();
            clearAutoStrafe(bytes);
            state().wasOnGround = false;
            state().shouldJump = false;
            return;
        }

        // m_fFlags (C_BaseEntity + 0x3F8), FL_ONGROUND = bit 0.
        const auto flags = *reinterpret_cast<std::uint32_t*>(bytes + kFlagsOffset);
        const bool onGround = (flags & 1) != 0;
        const bool startedAirMove = state().wasOnGround && !onGround;

        if (onGround) {
            if (!state().shouldJump && shouldSendJumpThisTick()) {
                writeJumpButton(true);
                injectJump(bytes);
                state().shouldJump = true;
            }
        } else {
            if (state().shouldJump) {
                releaseJump(bytes);
                state().shouldJump = false;
            } else {
                releaseJump();
            }
        }

        state().wasOnGround = onGround;

        // Auto-strafe optimization when in the air.
        if (GET_CONFIG_VAR(bhop_vars::AutoStrafe) && !onGround && jumpHeld) {
            applyAutoStrafe(bytes, startedAirMove);
        } else {
            clearAutoStrafe(bytes);
        }
    }

private:
    // IN_JUMP = 0x2. Inject the jump bit into the movement-services button
    // mask so CS2 processes a jump command this frame.
    void injectJump(std::byte* pawnBytes) noexcept
    {
        // Write IN_JUMP to all three button-state fields so CS2's next
        // input tick picks up the jump regardless of subtick timing.
        auto* moveSvcs = readMoveServices(pawnBytes);
        if (moveSvcs) {
            auto* buttons = reinterpret_cast<std::uint64_t*>(moveSvcs + kButtonsOffset);
            auto* queuedDown = reinterpret_cast<std::uint64_t*>(moveSvcs + kQueuedButtonDownMaskOffset);
            auto* queuedChange = reinterpret_cast<std::uint64_t*>(moveSvcs + kQueuedButtonChangeMaskOffset);
            *buttons |= kInJump;
            *queuedDown |= kInJump;
            *queuedChange |= kInJump;
        }
    }

    void releaseJump(std::byte* pawnBytes = nullptr) noexcept
    {
        writeJumpButton(false);
        if (!pawnBytes)
            return;

        auto* moveSvcs = readMoveServices(pawnBytes);
        if (!moveSvcs)
            return;

        auto* buttons = reinterpret_cast<std::uint64_t*>(moveSvcs + kButtonsOffset);
        auto* queuedDown = reinterpret_cast<std::uint64_t*>(moveSvcs + kQueuedButtonDownMaskOffset);
        auto* queuedChange = reinterpret_cast<std::uint64_t*>(moveSvcs + kQueuedButtonChangeMaskOffset);
        *buttons &= ~kInJump;
        *queuedDown &= ~kInJump;
        *queuedChange |= kInJump;
    }

    void applyAutoStrafe(std::byte* pawnBytes, bool startedAirMove) noexcept
    {
        // Read the current velocity to determine the optimal strafe direction.
        const auto& vel = *reinterpret_cast<cs2::Vector*>(
            pawnBytes + kAbsVelocityOffset);
        const float speed2d = fastSqrt(vel.x * vel.x + vel.y * vel.y);
        if (speed2d < 20.0f) {
            clearAutoStrafe(pawnBytes);
            return;
        }

        const bool pressingForward = isKeyHeld('W');
        const bool pressingBack = isKeyHeld('S');
        const bool pressingLeft = isKeyHeld('A');
        const bool pressingRight = isKeyHeld('D');
        if (!pressingForward && !pressingBack && !pressingLeft && !pressingRight) {
            clearAutoStrafe(pawnBytes);
            return;
        }

        auto* moveSvcs = readMoveServices(pawnBytes);
        if (!moveSvcs)
            return;

        const float sign = chooseSweepStrafeSign(startedAirMove, pressingLeft, pressingRight, pressingBack);
        const float strafeAmount = sign * kMoveScale;
        *reinterpret_cast<float*>(moveSvcs + kCmdLeftMoveOffset) = strafeAmount;
        *reinterpret_cast<float*>(moveSvcs + kLeftMoveOffset) = strafeAmount;
        state().autoStrafeActive = true;

        if (state().autoStrafeYawRemaining > 0.0f)
            state().autoStrafeYawRemaining -= nudgeViewYaw(sign, speed2d, state().autoStrafeYawRemaining);
    }

    void clearAutoStrafe(std::byte* pawnBytes) noexcept
    {
        if (!state().autoStrafeActive)
            return;

        auto* moveSvcs = readMoveServices(pawnBytes);
        if (!moveSvcs) {
            state().autoStrafeActive = false;
            state().autoStrafeYawRemaining = 0.0f;
            releaseInjectedSideButton();
            return;
        }

        *reinterpret_cast<float*>(moveSvcs + kCmdLeftMoveOffset) = 0.0f;
        *reinterpret_cast<float*>(moveSvcs + kLeftMoveOffset) = 0.0f;
        state().autoStrafeActive = false;
        state().autoStrafeYawRemaining = 0.0f;
        releaseInjectedSideButton();
    }

    [[nodiscard]] static std::byte* readMoveServices(std::byte* pawnBytes) noexcept
    {
        if (!isReadableMemory(pawnBytes + kMoveServicesOffset, sizeof(std::byte*)))
            return nullptr;

        auto* moveSvcs = *reinterpret_cast<std::byte**>(
            pawnBytes + kMoveServicesOffset);
        if (!isReadableMemory(moveSvcs, kMovementServicesReadableSize))
            return nullptr;
        return moveSvcs;
    }

    [[nodiscard]] cs2::C_CSPlayerPawn* getLocalPlayerPawn() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        if (localController && isReadableMemory(localController.rawPointer(), kControllerReadableSize)) {
            const auto handle = localController.playerPawnHandle();
            if (handle) {
                if (auto* pawn = static_cast<cs2::C_CSPlayerPawn*>(
                        hookContext.template make<EntitySystem>().getEntityFromHandle(*handle));
                    isReadableMemory(pawn, kPawnReadableSize)) {
                    return pawn;
                }
            }
        }

#if IS_WIN64()
        if (const auto clientDll = GetModuleHandleA("client.dll")) {
            if (auto* pawn = *reinterpret_cast<cs2::C_CSPlayerPawn**>(
                    reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kLocalPlayerPawn);
                isReadableMemory(pawn, kPawnReadableSize)) {
                return pawn;
            }
        }
#endif
        return nullptr;
    }

    [[nodiscard]] bool shouldSendJumpThisTick() const noexcept
    {
        if (!isHumanized())
            return true;
        return (randU8() & 0x0F) != 0;
    }

    static void writeJumpButton(bool down) noexcept
    {
#if IS_WIN64()
        const auto clientDll = GetModuleHandleA("client.dll");
        if (!clientDll)
            return;
        *reinterpret_cast<std::uint32_t*>(
            reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kJumpButton) =
            down ? kButtonStateDown : kButtonStateUp;
#endif
    }

    void writeInjectedSideButton(float sign) noexcept
    {
        if (sign < 0.0f) {
            writeButton(cs2::client_dll_offsets::kLeftButton, true);
            writeButton(cs2::client_dll_offsets::kRightButton, false);
            state().injectedSideButton = -1.0f;
        } else {
            writeButton(cs2::client_dll_offsets::kRightButton, true);
            writeButton(cs2::client_dll_offsets::kLeftButton, false);
            state().injectedSideButton = 1.0f;
        }
    }

    void releaseInjectedSideButton() noexcept
    {
        if (state().injectedSideButton < 0.0f)
            writeButton(cs2::client_dll_offsets::kLeftButton, false);
        else if (state().injectedSideButton > 0.0f)
            writeButton(cs2::client_dll_offsets::kRightButton, false);
        state().injectedSideButton = 0.0f;
    }

    static void writeButton(std::ptrdiff_t offset, bool down) noexcept
    {
#if IS_WIN64()
        const auto clientDll = GetModuleHandleA("client.dll");
        if (!clientDll)
            return;
        *reinterpret_cast<std::uint32_t*>(
            reinterpret_cast<std::byte*>(clientDll) + offset) =
            down ? kButtonStateDown : kButtonStateUp;
#else
        (void)offset; (void)down;
#endif
    }

    float chooseSweepStrafeSign(bool startedAirMove, bool pressingLeft, bool pressingRight, bool movingBack) noexcept
    {
        if (startedAirMove || !state().autoStrafeActive) {
            if (pressingLeft && !pressingRight)
                state().strafeTarget = -1.0f;
            else if (pressingRight && !pressingLeft)
                state().strafeTarget = 1.0f;
            else
                state().strafeTarget = state().strafeTarget >= 0.0f ? -1.0f : 1.0f;
            state().autoStrafeYawRemaining = kHopYawSweepDegrees;
        } else if (state().autoStrafeYawRemaining <= 0.0f) {
            state().strafeTarget = -state().strafeTarget;
            state().autoStrafeYawRemaining = kHopYawSweepDegrees;
        }

        return movingBack ? -state().strafeTarget : state().strafeTarget;
    }

    static float nudgeViewYaw(float sign, float speed2d, float remaining) noexcept
    {
#if IS_WIN64()
        auto* angles = viewAngles();
        if (!angles)
            return 0.0f;

        float step = sweepStepAngle(speed2d);
        if (step > remaining)
            step = remaining;
        angles[1] = normalizeYaw(angles[1] + sign * step);
        return step;
#else
        (void)sign; (void)speed2d; (void)remaining;
        return 0.0f;
#endif
    }

    [[nodiscard]] static float sweepStepAngle(float speed2d) noexcept
    {
        const float speed = speed2d > 80.0f ? speed2d : 80.0f;
        float angle = 1000.0f / speed;
        if (angle < 1.6f)
            angle = 1.6f;
        if (angle > 4.5f)
            angle = 4.5f;
        return angle;
    }

    [[nodiscard]] static float* viewAngles() noexcept
    {
#if IS_WIN64()
        const auto clientDll = GetModuleHandleA("client.dll");
        if (!clientDll)
            return nullptr;
        auto* angles = reinterpret_cast<float*>(
            reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kViewAngles);
        if (!isReadableMemory(angles, sizeof(float) * 3))
            return nullptr;
        return angles;
#else
        return nullptr;
#endif
    }

    [[nodiscard]] static float normalizeYaw(float yaw) noexcept
    {
        while (yaw > 180.0f)
            yaw -= 360.0f;
        while (yaw < -180.0f)
            yaw += 360.0f;
        return yaw;
    }

    [[nodiscard]] bool isHumanized() const noexcept
    {
        return GET_CONFIG_VAR(bhop_vars::Humanization);
    }

    [[nodiscard]] static bool isKeyHeld(int vk) noexcept
    {
#if IS_WIN64()
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
#else
        return false;
#endif
    }

    [[nodiscard]] static bool isReadableMemory(const void* address, std::size_t size) noexcept
    {
#if IS_WIN64()
        if (!address || size == 0)
            return false;

        MEMORY_BASIC_INFORMATION info{};
        if (!VirtualQuery(address, &info, sizeof(info)))
            return false;
        if (info.State != MEM_COMMIT)
            return false;
        if ((info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0)
            return false;

        const auto start = reinterpret_cast<std::uintptr_t>(address);
        const auto end = start + size;
        const auto regionEnd = reinterpret_cast<std::uintptr_t>(info.BaseAddress) + info.RegionSize;
        return end > start && end <= regionEnd;
#else
        return address != nullptr;
#endif
    }

    [[nodiscard]] static std::uint8_t randU8() noexcept
    {
        static std::uint32_t s = 0x92D68CA2;
        s ^= s << 13; s ^= s >> 17; s ^= s << 5;
        return static_cast<std::uint8_t>(s);
    }

    [[nodiscard]] static float fastSqrt(float x) noexcept
    {
        if (x <= 0.0f) return 0.0f;
        std::int32_t i;
        const float x2 = x * 0.5f;
        auto y = x;
        i = *reinterpret_cast<std::int32_t*>(&y);
        i = 0x5f3759df - (i >> 1);
        y = *reinterpret_cast<float*>(&i);
        y = y * (1.5f - (x2 * y * y));
        return 1.0f / y;
    }

    [[nodiscard]] BHopState& state() const noexcept
    {
        return hookContext.featuresStates().bhopState;
    }

    // Schema offsets verified against a2x/cs2-dumper build 14171.
    static constexpr std::ptrdiff_t kFlagsOffset{0x3F4};        // C_BaseEntity::m_fFlags
    static constexpr std::ptrdiff_t kOnGroundLastTick{0x1B60};
    static constexpr std::ptrdiff_t kMoveServicesOffset{0x1248};
    static constexpr std::ptrdiff_t kButtonsOffset{0x50};
    static constexpr std::ptrdiff_t kQueuedButtonDownMaskOffset{0x70};
    static constexpr std::ptrdiff_t kQueuedButtonChangeMaskOffset{0x78};
    static constexpr std::ptrdiff_t kCmdForwardMoveOffset{0x1A0};
    static constexpr std::ptrdiff_t kCmdLeftMoveOffset{0x1A4};
    static constexpr std::ptrdiff_t kForwardMoveOffset{0x1C0};
    static constexpr std::ptrdiff_t kLeftMoveOffset{0x1C4};
    static constexpr std::ptrdiff_t kAbsVelocityOffset{0x3F8};
    static constexpr float kMoveScale{450.0f};
    static constexpr float kHopYawSweepDegrees{55.0f};
    static constexpr std::size_t kPawnReadableSize{0x1228};
    static constexpr std::size_t kControllerReadableSize{0x910};
    static constexpr std::size_t kMovementServicesReadableSize{0x244};
    static constexpr std::uint64_t kInJump{0x2ULL};
    static constexpr std::uint32_t kButtonStateDown{65537};
    static constexpr std::uint32_t kButtonStateUp{256};

    HookContext& hookContext;
    bool enabled;
};
