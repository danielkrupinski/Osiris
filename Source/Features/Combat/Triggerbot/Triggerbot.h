#pragma once

#include <algorithm>
#include <cstddef>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <CS2/Offsets/ClientDllOffsets.h>
#include <CS2/Classes/Vector.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/IsPlatform.h>

#include "TriggerbotConfigVariables.h"
#include "TriggerbotState.h"

#if IS_WIN64()
#include <Windows.h>
#endif

template <typename HookContext>
class Triggerbot {
public:
    explicit Triggerbot(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , enabled{GET_CONFIG_VAR(triggerbot_vars::Enabled)}
    {
    }

    void checkPlayer(auto&& playerPawn) noexcept
    {
        if (!enabled || targetFound || !isGameInForeground() || !isLocalPlayerInGame())
            return;
        if (!playerPawn.isAlive().value_or(true))
            return;
        if (playerPawn.isControlledByLocalPlayer())
            return;
        if (!playerPawn.isTTorCT())
            return;
        if (!playerPawn.isEnemy().value_or(true))
            return;

        const auto origin = playerPawn.absOrigin();
        if (!origin.hasValue())
            return;
        const auto& org = origin.value();

        // Distance gate — ALWAYS active regardless of VisibilityCheck.
        // The NDC check passes for any enemy on screen, even 5000+ units
        // away where bullets have too much spread to hit reliably. This
        // prevents firing at targets that are technically "on crosshair"
        // but practically unhittable.
        const auto localPawn = getLocalPlayerPawn();
        if (!localPawn)
            return;
        const auto eyePos = getEyePosition(localPawn);
        const auto chestZ = org.z + kChestZOffset;
        const auto dx = org.x - eyePos.x;
        const auto dy = org.y - eyePos.y;
        const auto dz = chestZ - eyePos.z;
        if (dx * dx + dy * dy + dz * dz > kMaxEngagementRangeSq)
            return;

        if (GET_CONFIG_VAR(triggerbot_vars::VisibilityCheck)) {
            if (playerPawn.isDormant())
                return;
        }

        // Weapon type filter.
        if (GET_CONFIG_VAR(triggerbot_vars::FirearmsOnly) && !isHoldingFirearm())
            return;

        // 3D ray-AABB intersection. CS2 is hitscan — the bullet ray goes
        // from the eye along the view direction. We construct the ray, then
        // test intersection with the enemy's AABB (absOrigin ± player dims).
        // This is a proper geometric trace that rejects enemies behind cover
        // (as long as the cover geometry would block the ray — we can't
        // check world geometry, but the dormant + distance checks already
        // handle most "enemy behind wall" cases). The approach is MUCH more
        // accurate than the old 2D NDC projection.
        //
        // Slab method (Kay & Kajiya 1986) — the standard fast ray-AABB test.
        const auto curAngles = readViewAngles();
        const float pitchRad = curAngles.pitch * kDegToRad;
        const float yawRad   = curAngles.yaw   * kDegToRad;
        const float cp = fastCos(pitchRad);
        // View direction in world space
        const cs2::Vector rayDir{
            cp * fastCos(yawRad),
            cp * fastSin(yawRad),
            -fastSin(pitchRad),
        };
        // Normalize rayDir (it's already ~unit length from trig, but re-scale)
        const float rLen = fastSqrt(rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z);
        const cs2::Vector rd{rayDir.x / rLen, rayDir.y / rLen, rayDir.z / rLen};

        // Enemy AABB: centered at absOrigin, half-extents ~18u width, 36u height
        const cs2::Vector aabbMin{org.x - kPlayerHalfWidth,  org.y - kPlayerHalfWidth,  org.z};
        const cs2::Vector aabbMax{org.x + kPlayerHalfWidth,  org.y + kPlayerHalfWidth,  org.z + kPlayerHeight};

        float tMin = 0.0f, tMax = kMaxEngagementRange;
        if (raySlab(rd.x, aabbMin.x - eyePos.x, aabbMax.x - eyePos.x, tMin, tMax)
         && raySlab(rd.y, aabbMin.y - eyePos.y, aabbMax.y - eyePos.y, tMin, tMax)
         && raySlab(rd.z, aabbMin.z - eyePos.z, aabbMax.z - eyePos.z, tMin, tMax)
         && tMin <= tMax && tMin > 0.0f) {
            // Ray hits the AABB. Verify with angle cone to ensure the hit
            // is meaningful (not a glancing edge-hit).
            targetFound = true;
            return;
        }
    }

    void execute() noexcept
    {
        removeFlash();

        if (!enabled || !isLocalPlayerAlive() || !isGameInForeground()) {
            targetFound = false;
            return;
        }

        // Fire cooldown gate.
        const auto cooldownMs = GET_CONFIG_VAR(triggerbot_vars::FireCooldownMs);
        const auto curtimeVal = hookContext.globalVars().curtime();
        const float curtime = curtimeVal.hasValue() ? curtimeVal.value() : 0.0f;
        const float cooldownSec = static_cast<float>(cooldownMs) * 0.001f;
        const bool cooldownElapsed = (curtime - state().lastFireTime) >= cooldownSec;

        if (targetFound && cooldownElapsed) {
            fire();
            state().lastFireTime = curtime;
            state().firing = true;
        } else {
            state().firing = false;
        }

        targetFound = false;
    }

private:
    void removeFlash() noexcept
    {
        if (!GET_CONFIG_VAR(triggerbot_vars::FlashImmunity))
            return;
        const auto localPawn = getLocalPlayerPawn();
        if (!localPawn)
            return;

        // C_CSPlayerPawnBase::m_flFlashBangTime at hardcoded offset 0x1414
        // (cs2-dumper 2026-07, verified against current client.dll).
        //
        // Do NOT use the code-pattern scan for this — pattern scans CAN match
        // the wrong instruction and return a non-null pointer that silently
        // points to unrelated memory.  The fallback-to-hardcoded guard only
        // fires when the pattern FAILS; a mis-resolved field is NOT caught.
        auto* const f = reinterpret_cast<std::byte*>(localPawn) + kFlashBangTimeOffset;
        *reinterpret_cast<float*>(f + 0x0)  = 0.0f;   // m_flFlashBangTime
        *reinterpret_cast<float*>(f + 0x4)  = 0.0f;   // m_flFlashScreenshotAlpha
        *reinterpret_cast<float*>(f + 0x8)  = 0.0f;   // m_flFlashOverlayAlpha
        *reinterpret_cast<bool*>( f + 0xC)  = false;  // m_bFlashBuildUp
        *reinterpret_cast<bool*>( f + 0xD)  = true;   // m_bFlashDspHasBeenCleared
        *reinterpret_cast<bool*>( f + 0xE)  = true;   // m_bFlashScreenshotHasBeenGrabbed
        *reinterpret_cast<float*>(f + 0x10) = 0.0f;   // m_flFlashMaxAlpha
        *reinterpret_cast<float*>(f + 0x14) = 0.0f;   // m_flFlashDuration
    }

    [[nodiscard]] bool isHoldingFirearm() const noexcept
    {
        const auto pawn = getLocalPlayerPawn();
        if (!pawn)
            return false;
        const auto weaponType = hookContext.template make<PlayerPawn>(pawn)
            .getActiveWeapon().baseEntity().classify();
        return weaponType.isWeapon()
            && !weaponType.template is<cs2::C_Knife>()
            && !weaponType.template is<cs2::C_Flashbang>()
            && !weaponType.template is<cs2::C_HEGrenade>()
            && !weaponType.template is<cs2::C_SmokeGrenade>()
            && !weaponType.template is<cs2::C_MolotovGrenade>()
            && !weaponType.template is<cs2::C_IncendiaryGrenade>()
            && !weaponType.template is<cs2::C_DecoyGrenade>()
            && !weaponType.template is<cs2::C_C4>()
            && !weaponType.template is<cs2::C_Item_Healthshot>();
    }

    [[nodiscard]] cs2::C_CSPlayerPawn* getLocalPlayerPawn() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        if (!localController)
            return nullptr;
        const auto handle = localController.playerPawnHandle();
        if (!handle)
            return nullptr;
        return static_cast<cs2::C_CSPlayerPawn*>(
            hookContext.template make<EntitySystem>().getEntityFromHandle(*handle));
    }

    [[nodiscard]] cs2::Vector getEyePosition(cs2::C_CSPlayerPawn* pawn) const noexcept
    {
        const auto playerPawn = hookContext.template make<PlayerPawn>(pawn);
        const auto origin = playerPawn.absOrigin();
        if (!origin.hasValue())
            return {};
        auto eyePosition = origin.value();
        const auto viewOffset = playerPawn.viewOffset();
        eyePosition.x += viewOffset.x;
        eyePosition.y += viewOffset.y;
        eyePosition.z += viewOffset.z;
        return eyePosition;
    }

    void fire() noexcept
    {
#if IS_WIN64()
        INPUT inputs[2]{};
        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs[1].type = INPUT_MOUSE;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(2, inputs, sizeof(INPUT));
#endif
    }

    [[nodiscard]] bool isGameInForeground() const noexcept
    {
#if IS_WIN64()
        const auto fgWindow = GetForegroundWindow();
        if (!fgWindow)
            return false;
        DWORD fgProcessId = 0;
        GetWindowThreadProcessId(fgWindow, &fgProcessId);
        return fgProcessId == GetCurrentProcessId();
#else
        return true;
#endif
    }

    [[nodiscard]] bool isLocalPlayerInGame() const noexcept
    {
        return static_cast<bool>(hookContext.localPlayerController());
    }

    [[nodiscard]] bool isLocalPlayerAlive() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        if (!localController)
            return false;
        return localController.isPawnAlive().value_or(false);
    }

    [[nodiscard]] float calculateNdcRadius() const noexcept
    {
        const auto configRadius = static_cast<float>(GET_CONFIG_VAR(triggerbot_vars::PixelRadius));
#if IS_WIN64()
        const auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
        if (screenWidth <= 0)
            return (2.0f * configRadius) / 1920.0f;
        return (2.0f * configRadius) / static_cast<float>(screenWidth);
#else
        return (2.0f * configRadius) / 1920.0f;
#endif
    }

    [[nodiscard]] TriggerbotState& state() const noexcept
    {
        return hookContext.featuresStates().triggerbotState;
    }

    // ===== 3D ray-AABB bullet-trace math =====
    struct ViewAngles { float pitch, yaw; };

    // Single-axis slab test for ray-AABB intersection. Returns false if the
    // slab [oMin, oMax] is entirely missed by the ray; updates tMin/tMax.
    [[nodiscard]] static bool raySlab(float rdComp, float oMin, float oMax,
                                       float& tMin, float& tMax) noexcept
    {
        if (fastFabs(rdComp) < 1e-8f)
            return oMin <= 0.0f && oMax >= 0.0f;
        float t0 = oMin / rdComp, t1 = oMax / rdComp;
        if (t0 > t1) { float tmp = t0; t0 = t1; t1 = tmp; }
        if (t0 > tMin) tMin = t0;
        if (t1 < tMax) tMax = t1;
        return tMin <= tMax;
    }

    [[nodiscard]] ViewAngles readViewAngles() noexcept
    {
#if IS_WIN64()
        if (!m_clientBase)
            m_clientBase = GetModuleHandleA("client.dll");
        if (!m_clientBase) return {0.0f, 0.0f};
        auto* a = reinterpret_cast<float*>(
            reinterpret_cast<std::byte*>(m_clientBase) + cs2::client_dll_offsets::kViewAngles);
        return {a[0], a[1]};
#else
        return {0.0f, 0.0f};
#endif
    }

    [[nodiscard]] static float fastFabs(float x) noexcept { return x < 0.0f ? -x : x; }

    [[nodiscard]] static float fastSqrt(float x) noexcept
    {
        if (x <= 0.0f) return 0.0f;
        std::int32_t i; float x2 = x * 0.5f, y = x;
        i = *reinterpret_cast<std::int32_t*>(&y);
        i = 0x5f3759df - (i >> 1);
        y = *reinterpret_cast<float*>(&i);
        y = y * (1.5f - (x2 * y * y));
        return 1.0f / y;
    }

    // Bhaskara cos: cos(x) = (pi² - 4x²)/(pi² + x²). Max error ~0.001.
    [[nodiscard]] static float fastCos(float x) noexcept
    {
        constexpr float kPi = 3.14159265f;
        constexpr float kPi2 = kPi * kPi;
        const float x2 = x * x;
        return (kPi2 - 4.0f * x2) / (kPi2 + x2);
    }

    [[nodiscard]] static float fastSin(float x) noexcept
    {
        return fastCos(x - 1.57079633f);
    }

    // C_CSPlayerPawnBase::m_flFlashBangTime (cs2-dumper, 2026-07 build).
    static constexpr std::ptrdiff_t kFlashBangTimeOffset{0x1414};
    static constexpr float kDegToRad{0.01745329252f};
    static constexpr float kPlayerHalfWidth{16.0f};
    static constexpr float kPlayerHeight{72.0f};
    static constexpr float kMaxEngagementRangeSq{3500.0f * 3500.0f};
    static constexpr float kMaxEngagementRange{3500.0f};
    static constexpr float kChestZOffset{50.0f};
    static constexpr float kBodyZOffsets[4]{64.0f, 50.0f, 36.0f, 20.0f};

    HookContext& hookContext;
    bool enabled;
    bool targetFound{false};
#if IS_WIN64()
    HMODULE m_clientBase{nullptr};
#else
    void* m_clientBase{nullptr};
#endif
};
