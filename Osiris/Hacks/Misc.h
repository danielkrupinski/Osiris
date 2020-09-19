#pragma once

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
    void chokePackets(bool& sendPacket, UserCmd* cmd) noexcept;
    void autoReload(UserCmd* cmd) noexcept;
    void revealRanks(UserCmd* cmd) noexcept;
    void autoStrafe(UserCmd* cmd) noexcept;
    void removeCrouchCooldown(UserCmd* cmd) noexcept;
    void moonwalk(UserCmd* cmd) noexcept;
    void playHitSound(GameEvent& event) noexcept;
    void killSound(GameEvent& event) noexcept;
    void purchaseList(GameEvent* event = nullptr) noexcept;
    void oppositeHandKnife(FrameStage stage) noexcept;
    void runReportbot() noexcept;
    void resetReportbot() noexcept;
    void preserveKillfeed(bool roundStart = false) noexcept;

    inline int teamDamage = 0;
    inline int teamKills = 0;

    void jumpbug(UserCmd* cmd) noexcept;
    void teamDamageCounter(GameEvent* event) noexcept;
    void drawAimbotFov() noexcept;
    void showVelocity() noexcept;
    void drawBombDamage() noexcept;
    void playerBlocker(UserCmd* cmd) noexcept;
    void doorSpam(UserCmd* cmd) noexcept;
    void chatSpam() noexcept;
    void pure() noexcept;
    void MLP() noexcept;
    void setName(bool set) noexcept;
    void fakeItem(bool = false) noexcept;
    void buyBot(GameEvent* event) noexcept;
    void customViewmodelPosition(FrameStage stage) noexcept;
    void viewBob(FrameStage stage) noexcept;
    void fakeDuck(UserCmd* cmd, bool& sendPacket) noexcept;
    void DrawInaccuracy(ImDrawList* draw) noexcept;
}
