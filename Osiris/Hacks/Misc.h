#pragma once

#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/FrameStage.h"
#include "../SDK/UserCmd.h"
#include "../SDK/WeaponId.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/Entity.h"
#include "../SDK/Client.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"

namespace Misc
{
    void edgejump(UserCmd* cmd) noexcept;
    void slowwalk(UserCmd* cmd) noexcept;
    void inverseRagdollGravity() noexcept;
    void updateClanTag(bool = false) noexcept;
    void spectatorList() noexcept;
    void sniperCrosshair() noexcept;
    void recoilCrosshair() noexcept;
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
    void chokePackets(bool& sendPacket) noexcept;
    void autoReload(UserCmd* cmd) noexcept;
    void revealRanks(UserCmd* cmd) noexcept;
    void autoStrafe(UserCmd* cmd) noexcept;
    void removeCrouchCooldown(UserCmd* cmd) noexcept;
    void moonwalk(UserCmd* cmd) noexcept;
    void playHitSound(GameEvent& event) noexcept;
}
