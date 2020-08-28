#pragma once
#include "../SDK/Vector.h"

enum class FrameStage;
class GameEvent;
struct ImDrawList;
struct UserCmd;

namespace Misc
{
    void edgejump(UserCmd* cmd) noexcept;
    void slowwalk(UserCmd* cmd) noexcept;
    void inverseRagdollGravity() noexcept;
    void updateClanTag(bool = false) noexcept;
    void spectatorList() noexcept;
    void noscopeCrosshair(ImDrawList* drawlist) noexcept;
    void recoilCrosshair(ImDrawList* drawList) noexcept;
    void watermark() noexcept;
    void prepareRevolver(UserCmd*) noexcept;
    void fastPlant(UserCmd*) noexcept;
    void drawBombTimer() noexcept;
    void stealNames() noexcept;
    void disablePanoramablur() noexcept;
    void quickReload(UserCmd*) noexcept;
    bool changeName(bool, const char*, float) noexcept;
    void bunnyHop(UserCmd*) noexcept;
    void fakeBan(bool = false) noexcept;
    void nadePredict() noexcept;
    void quickHealthshot(UserCmd*) noexcept;
    void fixTabletSignal() noexcept;
    void fakePrime() noexcept;
    void killMessage(GameEvent& event) noexcept;
    void fixMovement(UserCmd* cmd, float yaw) noexcept;
    void antiAfkKick(UserCmd* cmd) noexcept;
    void fixAnimationLOD(FrameStage stage) noexcept;
    void autoPistol(UserCmd* cmd) noexcept;
    void autoReload(UserCmd* cmd) noexcept;
    void revealRanks(UserCmd* cmd) noexcept;
    void autoStrafe(UserCmd* cmd) noexcept;
    void removeCrouchCooldown(UserCmd* cmd) noexcept;
    void moonwalk(UserCmd* cmd) noexcept;
    void playHitSound(GameEvent& event) noexcept;
    void killSound(GameEvent& event) noexcept;
    void purchaseList(GameEvent* event = nullptr) noexcept;
    void StatusBar() noexcept;
    void DrawInaccuracy(ImDrawList* draw) noexcept;
	void ShotsCout(GameEvent* event = nullptr, int bestRageDmg = 0, int bestRageChance = 0, Vector quickpeekvector = {0, 0, 0}) noexcept;
	void fakeDuck(UserCmd* cmd, bool& sendPacket) noexcept;
    float RandomFloat(float min, float max) noexcept;
    void chokePackets(bool& sendPacket, UserCmd* cmd) noexcept;
	void drawStartPos(ImDrawList* dl, Vector &quickpeekstartpos) noexcept;
	void quickpeek(UserCmd* cmd, Vector &quickpeekstartpos) noexcept;
	void drawWallbangVector(ImDrawList* dl, Vector &wallbangVector, int wallDmg, int wallChance) noexcept;
    void oppositeHandKnife(FrameStage stage) noexcept;
    void runReportbot() noexcept;
    void resetReportbot() noexcept;
}
