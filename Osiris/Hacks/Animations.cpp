#include "Animations.h"

#include "../Memory.h"
#include "../MemAlloc.h"
#include "../Interfaces.h"

#include "../SDK/LocalPlayer.h"
#include "../SDK/Cvar.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/ConVar.h"
#include "../SDK/Input.h"

Animations::Datas Animations::data;

void Animations::update(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;
    data.viewangles = cmd->viewangles;
    data.sendPacket = sendPacket;
}

void Animations::fake() noexcept
{
    static AnimState* fakeanimstate = nullptr;
    static bool updatefakeanim = true;
    static bool initfakeanim = true;
    static float spawnTime = 0.f;

    if (!interfaces->engine->isInGame())
    {
        updatefakeanim = true;
        initfakeanim = true;
        spawnTime = 0.f;
        fakeanimstate = nullptr;
    }

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (spawnTime != localPlayer.get()->spawnTime() || updatefakeanim)
    {
        spawnTime = localPlayer.get()->spawnTime();
        initfakeanim = false;
        updatefakeanim = false;
    }

    if (!initfakeanim)
    {
        fakeanimstate = static_cast<AnimState*>(memory->memalloc->Alloc(sizeof(AnimState)));

        if (fakeanimstate != nullptr)
            localPlayer.get()->CreateState(fakeanimstate);

        initfakeanim = true;
    }
    if (data.sendPacket)
    {
        std::array<AnimationLayer, 15> networked_layers;

        std::memcpy(&networked_layers, localPlayer.get()->animOverlays(), sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
        auto backup_abs_angles = localPlayer.get()->getAbsAngle();
        auto backup_poses = localPlayer.get()->pose_parameters();

        *(uint32_t*)((uintptr_t)localPlayer.get() + 0xA68) = 0;

        localPlayer.get()->UpdateState(fakeanimstate, data.viewangles);
        localPlayer.get()->InvalidateBoneCache();
        memory->setAbsAngle(localPlayer.get(), Vector{ 0, fakeanimstate->GoalFeetYaw, 0 });
        std::memcpy(localPlayer.get()->animOverlays(), &networked_layers, sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());

        data.gotMatrix = localPlayer.get()->setupBones(data.fakematrix, 256, 0x7FF00, memory->globalVars->currenttime);
        const auto origin = localPlayer.get()->getRenderOrigin();
        if (data.gotMatrix)
        {
            for (auto& i : data.fakematrix)
            {
                i[0][3] -= origin.x;
                i[1][3] -= origin.y;
                i[2][3] -= origin.z;
            }
        }
        std::memcpy(localPlayer.get()->animOverlays(), &networked_layers, sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
        localPlayer.get()->pose_parameters() = backup_poses;
        memory->setAbsAngle(localPlayer.get(), Vector{ 0,backup_abs_angles.y,0 });
    }
}

void Animations::real() noexcept
{
    static auto jigglebones = interfaces->cvar->findVar("r_jiggle_bones");
    jigglebones->setValue(0);

    if (!localPlayer)
        return;

    if (!localPlayer->isAlive())
    {
        localPlayer.get()->ClientSideAnimation() = true;
        return;
    }

    static auto backup_poses = localPlayer.get()->pose_parameters();
    static auto backup_abs = localPlayer.get()->getAnimstate()->GoalFeetYaw;

    if (!memory->input->isCameraInThirdPerson) {
        localPlayer.get()->ClientSideAnimation() = true;
        localPlayer.get()->UpdateClientSideAnimation();
        localPlayer.get()->ClientSideAnimation() = false;
        return;
    }

    static std::array<AnimationLayer, 15> networked_layers;

    while (localPlayer.get()->getAnimstate()->LastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
        localPlayer.get()->getAnimstate()->LastClientSideAnimationUpdateFramecount -= 1;

    static int old_tick = 0;
    if (old_tick != memory->globalVars->tickCount)
    {
        old_tick = memory->globalVars->tickCount;
        std::memcpy(&networked_layers, localPlayer.get()->animOverlays(), sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
        localPlayer.get()->ClientSideAnimation() = true;
        localPlayer.get()->UpdateState(localPlayer->getAnimstate(), data.viewangles);
        localPlayer.get()->UpdateClientSideAnimation();
        localPlayer.get()->ClientSideAnimation() = false;
        if (data.sendPacket)
        {
            backup_poses = localPlayer.get()->pose_parameters();
            backup_abs = localPlayer.get()->getAnimstate()->GoalFeetYaw;
        }
    }
    localPlayer.get()->getAnimstate()->FeetYawRate = 0.f;
    memory->setAbsAngle(localPlayer.get(), Vector{ 0,backup_abs,0 });
    localPlayer.get()->getAnimstate()->UnknownFraction = 0.f;
    std::memcpy(localPlayer.get()->animOverlays(), &networked_layers, sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
    localPlayer.get()->pose_parameters() = backup_poses;
}