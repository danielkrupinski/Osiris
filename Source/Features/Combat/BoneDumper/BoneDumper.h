#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include <CS2/Classes/Vector.h>
#include <CS2/Constants/BoneIndex.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/StringBuilder.h>

#include "BoneDumperConfigVariables.h"
#include "BoneDumperState.h"

#if IS_WIN64()
#include <Windows.h>
#endif

// Walks visible enemies each frame, projects their bones to clip space, and
// writes YOLO-format bounding boxes to a JSON file. A separate Python script
// captures the screen and pairs frames with these labels — auto-generating a
// training dataset for visual aimbot models with zero manual labeling effort.
//
// Output: C:/temp/cs2_labels/latest.json (atomically replaced each frame)
//
// JSON schema:
//   { "t": <curtime>, "frame": <counter>,
//     "objects": [ {"c": <class>, "b": [cx, cy, w, h]}, ... ] }
//   class 0 = head, class 1 = body. cx/cy/w/h normalized to [0,1].
template <typename HookContext>
class BoneDumper {
public:
    explicit BoneDumper(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , enabled{GET_CONFIG_VAR(bone_dumper_vars::Enabled)}
    {
    }

    void beginFrame() noexcept
    {
        if (!enabled)
            return;

        const auto curOpt = hookContext.globalVars().curtime();
        const float curtime = curOpt.hasValue() ? curOpt.value() : 0.0f;
        // curtime resets to 0 on disconnect / map change / new match. Without
        // this guard the throttle delta (curtime - lastDumpCurtime) goes
        // negative and we never resume writing.
        if (curtime < state().lastDumpCurtime)
            state().lastDumpCurtime = 0.0f;
        if (curtime - state().lastDumpCurtime < kMinDumpInterval) {
            shouldDump = false;
            return;
        }
        shouldDump = true;
        state().lastDumpCurtime = curtime;
        objectCount = 0;
        cachedCurtime = curtime;
        bufferUsed = 0;
        // Reset diagnostic counters for this frame
        stats = Stats{};
        appendStr("{\"t\":");
        appendTime(cachedCurtime);
        appendStr(",\"frame\":");
        appendUint64(state().frameCounter);
        appendStr(",\"objects\":[");
    }

    void addEnemy(auto&& playerPawn) noexcept
    {
        if (!enabled || !shouldDump)
            return;
        stats.iter++;
        // Same permissive filters as Aimbot: value_or(true) accepts the player
        // when the schema lookup returns an empty Optional (otherwise we'd
        // reject everyone if the alive-flag offset goes stale after a patch).
        if (!playerPawn.isAlive().value_or(true))
            return;
        stats.alive++;
        if (playerPawn.isControlledByLocalPlayer())
            return;
        stats.notLocal++;
        if (!playerPawn.isTTorCT())
            return;
        stats.team++;
        if (!playerPawn.isEnemy().value_or(true))
            return;
        stats.enemy++;
        if (playerPawn.isDormant())
            return;
        stats.notDormant++;

        const auto origin = playerPawn.absOrigin();
        if (!origin.hasValue())
            return;
        const auto& org = origin.value();
        stats.hasOrigin++;

        // Try bone array first. If bones return zero valid points (e.g.,
        // the bone offset is stale after a CS2 update), fall back to
        // viewOffset-scaled body points — same trick Aimbot uses to stay
        // robust without depending on the unverified bone array offset.
        BoneScreen pts[kBonePointCount]{};
        std::size_t valid = 0;
        for (std::size_t i = 0; i < kBonePointCount; ++i) {
            const auto bp = playerPawn.bonePosition(kBonesToProject[i]);
            if (!bp.hasValue())
                continue;
            // Sanity: bones should be within ~100 units of the player origin.
            const auto& bv = bp.value();
            const float bx = bv.x - org.x, by = bv.y - org.y, bz = bv.z - org.z;
            if (bx * bx + by * by + bz * bz > 10000.0f)
                continue; // garbage from stale offset, ignore
            float ndcX, ndcY;
            if (!toNdc(bv, ndcX, ndcY))
                continue;
            pts[valid].x = ndcX;
            pts[valid].y = ndcY;
            pts[valid].kind = kBoneKind[i];
            ++valid;
        }

        if (valid < 2) {
            // Bone read failed (or all off-screen). Synthesize a hull of
            // points spanning the player's volume: head/chest/stomach/pelvis
            // at the center axis PLUS ±halfwidth horizontal offsets at chest
            // height. Without the horizontal offsets the projected bbox would
            // collapse to zero width (all points on the same vertical line),
            // and the bodyW < 0.005 reject would drop them.
            const auto viewOff = playerPawn.viewOffset();
            const float eyeZ = viewOff.z; // ~64 standing, ~46 crouched
            if (eyeZ < 10.0f) return; // unusable viewOffset
            constexpr float kHalfWidth = 14.0f; // ~CS2 player shoulder half-width
            constexpr float kFootSpread = 6.0f;
            const float chestZ = eyeZ * 0.78f;
            const cs2::Vector fallbackPts[]{
                {org.x,               org.y,               org.z + eyeZ},     // head
                {org.x,               org.y,               org.z + chestZ},   // chest center
                {org.x + kHalfWidth,  org.y,               org.z + chestZ},   // shoulder +X
                {org.x - kHalfWidth,  org.y,               org.z + chestZ},   // shoulder -X
                {org.x,               org.y + kHalfWidth,  org.z + chestZ},   // shoulder +Y
                {org.x,               org.y - kHalfWidth,  org.z + chestZ},   // shoulder -Y
                {org.x,               org.y,               org.z + eyeZ * 0.56f}, // stomach
                {org.x + kFootSpread, org.y,               org.z},            // foot +X
                {org.x - kFootSpread, org.y,               org.z},            // foot -X
            };
            constexpr BoneKind fallbackKinds[]{
                BoneKind::Head,
                BoneKind::Body, BoneKind::Body, BoneKind::Body, BoneKind::Body, BoneKind::Body,
                BoneKind::Body, BoneKind::Body, BoneKind::Body,
            };
            valid = 0;
            constexpr std::size_t kFallbackCount = sizeof(fallbackPts) / sizeof(fallbackPts[0]);
            for (std::size_t i = 0; i < kFallbackCount && valid < kBonePointCount; ++i) {
                float ndcX, ndcY;
                if (!toNdc(fallbackPts[i], ndcX, ndcY))
                    continue;
                pts[valid].x = ndcX;
                pts[valid].y = ndcY;
                pts[valid].kind = fallbackKinds[i];
                ++valid;
            }
            if (valid < 2)
                return;
            stats.usedFallback++;
        } else {
            stats.usedBones++;
        }

        // Body bbox: tightest rect enclosing all valid bones.
        float bodyMinX = pts[0].x, bodyMaxX = pts[0].x;
        float bodyMinY = pts[0].y, bodyMaxY = pts[0].y;
        float headX = 0.0f, headY = 0.0f;
        bool haveHead = false;
        for (std::size_t i = 0; i < valid; ++i) {
            if (pts[i].x < bodyMinX) bodyMinX = pts[i].x;
            if (pts[i].x > bodyMaxX) bodyMaxX = pts[i].x;
            if (pts[i].y < bodyMinY) bodyMinY = pts[i].y;
            if (pts[i].y > bodyMaxY) bodyMaxY = pts[i].y;
            if (pts[i].kind == BoneKind::Head) {
                headX = pts[i].x;
                headY = pts[i].y;
                haveHead = true;
            }
        }

        // Convert NDC [-1,1] → image [0,1] (image y axis flips relative to NDC).
        const float bodyCx = (bodyMinX + bodyMaxX) * 0.25f + 0.5f;
        const float bodyCy = 1.0f - ((bodyMinY + bodyMaxY) * 0.25f + 0.5f);
        const float bodyW  = (bodyMaxX - bodyMinX) * 0.5f;
        const float bodyH  = (bodyMaxY - bodyMinY) * 0.5f;
        if (bodyW < 0.005f || bodyH < 0.01f) // too tiny
            return;

        if (haveHead) {
            const float headSize = bodyH * 0.18f;
            const float headCx = headX * 0.5f + 0.5f;
            const float headCy = 1.0f - (headY * 0.5f + 0.5f);
            writeObject(0, headCx, headCy, headSize, headSize);
        }
        writeObject(1, bodyCx, bodyCy, bodyW, bodyH);
    }

    void endFrame() noexcept
    {
        if (!enabled || !shouldDump)
            return;
        // Append diagnostic stats so the Python collector / user can see
        // exactly which filter is rejecting enemies when objects[] is empty.
        appendStr("],\"stats\":{\"iter\":");
        appendUint(static_cast<unsigned>(stats.iter));
        appendStr(",\"alive\":");
        appendUint(static_cast<unsigned>(stats.alive));
        appendStr(",\"notLocal\":");
        appendUint(static_cast<unsigned>(stats.notLocal));
        appendStr(",\"team\":");
        appendUint(static_cast<unsigned>(stats.team));
        appendStr(",\"enemy\":");
        appendUint(static_cast<unsigned>(stats.enemy));
        appendStr(",\"notDormant\":");
        appendUint(static_cast<unsigned>(stats.notDormant));
        appendStr(",\"hasOrigin\":");
        appendUint(static_cast<unsigned>(stats.hasOrigin));
        appendStr(",\"usedBones\":");
        appendUint(static_cast<unsigned>(stats.usedBones));
        appendStr(",\"usedFallback\":");
        appendUint(static_cast<unsigned>(stats.usedFallback));
        appendStr("}}\n");
        flushToFile();
        state().frameCounter++;
    }

private:
    enum class BoneKind : std::uint8_t { Head, Body };

    struct BoneScreen {
        float x{0.0f};
        float y{0.0f};
        BoneKind kind{BoneKind::Body};
    };

    static constexpr cs2::BoneIndex kBonesToProject[]{
        cs2::BoneIndex::Head,
        cs2::BoneIndex::Neck,
        cs2::BoneIndex::Chest,
        cs2::BoneIndex::Stomach,
        cs2::BoneIndex::Pelvis,
        cs2::BoneIndex::LeftFoot,
        cs2::BoneIndex::RightFoot,
    };
    static constexpr BoneKind kBoneKind[]{
        BoneKind::Head, BoneKind::Body, BoneKind::Body, BoneKind::Body,
        BoneKind::Body, BoneKind::Body, BoneKind::Body,
    };
    static constexpr std::size_t kBonePointCount = sizeof(kBonesToProject) / sizeof(kBonesToProject[0]);
    static constexpr float kMinDumpInterval{1.0f / 30.0f}; // cap at 30Hz

    bool toNdc(const cs2::Vector& world, float& ndcX, float& ndcY) const noexcept
    {
        const auto clip = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(world);
        if (!clip.onScreen())
            return false;
        ndcX = clip.x / clip.w;
        ndcY = clip.y / clip.w;
        if (ndcX < -1.0f || ndcX > 1.0f || ndcY < -1.0f || ndcY > 1.0f)
            return false;
        return true;
    }

    void writeObject(int cls, float cx, float cy, float w, float h) noexcept
    {
        if (objectCount > 0)
            appendStr(",");
        appendStr("{\"c\":");
        appendUint(static_cast<unsigned>(cls));
        appendStr(",\"b\":[");
        appendNorm5(cx); appendStr(",");
        appendNorm5(cy); appendStr(",");
        appendNorm5(w);  appendStr(",");
        appendNorm5(h);
        appendStr("]}");
        ++objectCount;
    }

    // ===== CRT-free string formatting =====
    void appendChar(char c) noexcept
    {
        if (bufferUsed < kBufCapacity)
            state().buffer[bufferUsed++] = c;
    }

    void appendStr(const char* s) noexcept
    {
        while (*s && bufferUsed < kBufCapacity)
            state().buffer[bufferUsed++] = *s++;
    }

    void appendUint(unsigned v) noexcept
    {
        char tmp[12];
        std::size_t n = 0;
        if (v == 0) { appendChar('0'); return; }
        while (v && n < sizeof(tmp)) {
            tmp[n++] = static_cast<char>('0' + (v % 10));
            v /= 10;
        }
        while (n--)
            appendChar(tmp[n]);
    }

    void appendUint64(std::uint64_t v) noexcept
    {
        char tmp[24];
        std::size_t n = 0;
        if (v == 0) { appendChar('0'); return; }
        while (v && n < sizeof(tmp)) {
            tmp[n++] = static_cast<char>('0' + (v % 10));
            v /= 10;
        }
        while (n--)
            appendChar(tmp[n]);
    }

    // Format a [0,1]-normalized float with 5 decimal digits (e.g. 0.12345).
    void appendNorm5(float v) noexcept
    {
        if (v < 0.0f) { appendChar('-'); v = -v; }
        if (v > 9.99999f) v = 9.99999f;
        const unsigned scaled = static_cast<unsigned>(v * 100000.0f + 0.5f);
        const unsigned hi = scaled / 100000;
        const unsigned lo = scaled % 100000;
        appendUint(hi);
        appendChar('.');
        // pad lo with leading zeros to 5 chars
        if (lo < 10000) appendChar('0');
        if (lo < 1000)  appendChar('0');
        if (lo < 100)   appendChar('0');
        if (lo < 10)    appendChar('0');
        appendUint(lo);
    }

    // Format curtime (seconds) with 3 decimal digits.
    void appendTime(float t) noexcept
    {
        if (t < 0.0f) { appendChar('-'); t = -t; }
        const unsigned sec = static_cast<unsigned>(t);
        const unsigned ms  = static_cast<unsigned>((t - static_cast<float>(sec)) * 1000.0f + 0.5f);
        appendUint(sec);
        appendChar('.');
        if (ms < 100) appendChar('0');
        if (ms < 10)  appendChar('0');
        appendUint(ms);
    }

    void flushToFile() noexcept
    {
#if IS_WIN64()
        ensureDir();
        // Atomic-ish replace: write .tmp then rename. Python reader sees
        // either old or new contents, never a half-written file.
        HANDLE h = CreateFileA("C:/temp/cs2_labels/latest.json.tmp",
                                GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE)
            return;
        DWORD wrote = 0;
        WriteFile(h, state().buffer.data(), static_cast<DWORD>(bufferUsed), &wrote, nullptr);
        CloseHandle(h);
        MoveFileExA("C:/temp/cs2_labels/latest.json.tmp",
                    "C:/temp/cs2_labels/latest.json",
                    MOVEFILE_REPLACE_EXISTING);
#endif
    }

    static void ensureDir() noexcept
    {
#if IS_WIN64()
        CreateDirectoryA("C:/temp", nullptr);
        CreateDirectoryA("C:/temp/cs2_labels", nullptr);
#endif
    }

    [[nodiscard]] BoneDumperState& state() const noexcept
    {
        return hookContext.featuresStates().boneDumperState;
    }

    static constexpr std::size_t kBufCapacity = BoneDumperState::kBufferCapacity;

    // Per-frame diagnostic counters — written into latest.json so you can see
    // exactly where the filter chain rejects enemies when objects[] is empty.
    struct Stats {
        std::uint16_t iter{0};       // addEnemy was called
        std::uint16_t alive{0};      // passed alive check
        std::uint16_t notLocal{0};   // passed not-local check
        std::uint16_t team{0};       // passed team check
        std::uint16_t enemy{0};      // passed enemy check
        std::uint16_t notDormant{0}; // passed dormant check
        std::uint16_t hasOrigin{0};  // absOrigin available
        std::uint16_t usedBones{0};  // wrote a bbox via real bones
        std::uint16_t usedFallback{0}; // wrote a bbox via viewOffset fallback
    };

    HookContext& hookContext;
    bool enabled;
    bool shouldDump{false};
    std::size_t objectCount{0};
    std::size_t bufferUsed{0};
    float cachedCurtime{0.0f};
    Stats stats{};
};
