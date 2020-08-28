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
        memory->setAbsAngle(localPlayer.get(), Vector{ 0, fakeanimstate->m_flGoalFeetYaw, 0 });
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
    static auto backup_abs = localPlayer.get()->getAnimstate()->m_flGoalFeetYaw;

    if (!memory->input->isCameraInThirdPerson) {
        localPlayer.get()->ClientSideAnimation() = true;
        localPlayer.get()->UpdateClientSideAnimation();
        localPlayer.get()->ClientSideAnimation() = false;
        return;
    }

    static std::array<AnimationLayer, 15> networked_layers;

    while (localPlayer.get()->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
        localPlayer.get()->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount -= 1;

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
            backup_abs = localPlayer.get()->getAnimstate()->m_flGoalFeetYaw;
        }
    }
    localPlayer.get()->getAnimstate()->m_flGoalFeetYaw = 0.f;
    memory->setAbsAngle(localPlayer.get(), Vector{ 0,backup_abs,0 });
    localPlayer.get()->getAnimstate()->UnknownFraction = 0.f;
    std::memcpy(localPlayer.get()->animOverlays(), &networked_layers, sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
    localPlayer.get()->pose_parameters() = backup_poses;
}

void Animations::players() noexcept
{
    if (!localPlayer)
        return;

    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++)
    {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || !entity->getAnimstate())
            continue;

        if (!data.player[i].once)
        {
            data.player[i].poses = entity->pose_parameters();
            data.player[i].abs = entity->getAnimstate()->m_flGoalFeetYaw;
            data.player[i].simtime = 0;
            data.player[i].once = true;
        }

        if(data.player[i].networked_layers.empty())
            std::memcpy(&data.player[i].networked_layers, entity->animOverlays(), sizeof(AnimationLayer) * entity->getAnimationLayerCount());

        while (entity->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
            entity->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount -= 1;
        entity->InvalidateBoneCache();
        memory->setAbsOrigin(entity, entity->origin());
        *entity->getEffects() &= ~0x1000;
        *entity->getAbsVelocity() = entity->velocity();
        std::memcpy(&data.player[i].networked_layers, entity->animOverlays(), sizeof(AnimationLayer) * entity->getAnimationLayerCount());
        entity->ClientSideAnimation() = true;
 /*       if (entity->isOtherEnemy(localPlayer.get()) && config->ragebotExtra.resolver && data.player[i].chokedPackets >= 1 && localPlayer->isAlive())
            entity->getAnimstate()->GoalFeetYaw = Resolver::CalculateFeet(entity);*/
        entity->UpdateClientSideAnimation();
        entity->ClientSideAnimation() = false;
        if (data.player[i].simtime != entity->simulationTime())
        {
            data.player[i].chokedPackets = static_cast<int>((entity->simulationTime() - data.player[i].simtime) / memory->globalVars->intervalPerTick) - 1;
            data.player[i].simtime = entity->simulationTime();
            data.player[i].poses = entity->pose_parameters();
            data.player[i].abs = entity->getAnimstate()->m_flGoalFeetYaw;
        }
        entity->getAnimstate()->m_flFeetYawRate = 0.f;
        memory->setAbsAngle(entity, Vector{ 0,data.player[i].abs,0 });
        data.player[i].networked_layers[12].weight = std::numeric_limits<float>::epsilon();
        std::memcpy(entity->animOverlays(), &data.player[i].networked_layers, sizeof(AnimationLayer) * entity->getAnimationLayerCount());
        entity->pose_parameters() = data.player[i].poses;
        entity->InvalidateBoneCache();
        entity->setupBones(data.player[i].matrix, 256, 0x7FF00, memory->globalVars->currenttime);
        auto backup = data.lastest[i];
        auto boneCache = *(int**)(entity + 0x2910);
        auto countBones = *(int*)(entity + 0x291C);

        backup.boneCache = boneCache;
        backup.countBones = countBones;
        backup.mins = entity->getCollideable()->obbMins();
        backup.max = entity->getCollideable()->obbMaxs();
        backup.origin = entity->getAbsOrigin();
    }
}

void Animations::setupResolver(Entity* entity, Tickbase::Tick record) noexcept
{
   /* if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive())
        return;

    auto backup = data.backupResolver[entity->index()];
    auto boneCache = *(int**)(entity + 0x2910);
    auto countBones = *(int*)(entity + 0x291C);

    backup.boneCache = boneCache;
    backup.countBones = countBones;
    backup.mins = entity->getCollideable()->obbMins();
    backup.max = entity->getCollideable()->obbMaxs();
    backup.origin = entity->getAbsOrigin();
    entity->InvalidateBoneCache();
    memcpy(boneCache, record.matrix, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));

    entity->getCollideable()->obbMins() = record.mins;
    entity->getCollideable()->obbMaxs() = record.max;
    memory->setAbsOrigin(entity, record.origin);
    backup.hasBackup = true;*/
}

void Animations::setup(Entity* entity, Backtrack::Record record) noexcept
{
    if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive())
        return;
    auto backup = data.backup[entity->index()];
    auto boneCache = *(int**)(entity + 0x2910);
    auto countBones = *(int*)(entity + 0x291C);

    backup.boneCache = boneCache;
    backup.countBones = countBones;
    backup.mins = entity->getCollideable()->obbMins();
    backup.max = entity->getCollideable()->obbMaxs();
    backup.origin = entity->getAbsOrigin();
    entity->InvalidateBoneCache();
    memcpy(boneCache, record.matrix, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));

    entity->getCollideable()->obbMins() = record.mins;
    entity->getCollideable()->obbMaxs() = record.max;
    memory->setAbsOrigin(entity, record.origin);
    backup.hasBackup = true;
}

void Animations::finishSetup(Entity* entity) noexcept
{
    if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive())
        return;

    auto backup = data.lastest[entity->index()];
    if (data.backup[entity->index()].hasBackup)
    {
        auto boneCache = *(int**)(entity + 0x2910);
        entity->InvalidateBoneCache();
        memcpy(boneCache, backup.boneCache, sizeof(matrix3x4) * std::clamp(backup.countBones, 0, 256));

        entity->getCollideable()->obbMins() = backup.mins;
        entity->getCollideable()->obbMaxs() = backup.max;

        memory->setAbsOrigin(entity, backup.origin);
        data.backup[entity->index()].hasBackup = false;
    }
    else if (data.backupResolver[entity->index()].hasBackup)
    {
        auto boneCache = *(int**)(entity + 0x2910);
        entity->InvalidateBoneCache();
        memcpy(boneCache, backup.boneCache, sizeof(matrix3x4) * std::clamp(backup.countBones, 0, 256));

        entity->getCollideable()->obbMins() = backup.mins;
        entity->getCollideable()->obbMaxs() = backup.max;

        memory->setAbsOrigin(entity, backup.origin);
        data.backupResolver[entity->index()].hasBackup = false;
    }
}