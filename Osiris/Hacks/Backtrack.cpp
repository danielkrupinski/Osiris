#include "Backtrack.h"
#include "Aimbot.h"
#include "Chams.h"
#include "../Config.h"

std::deque<Backtrack::Record> Backtrack::records[65];
Backtrack::Cvars Backtrack::cvars;

void Backtrack::update(FrameStage stage) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!config.backtrack.enabled || !localPlayer || !localPlayer->isAlive()) {
        if (!records->empty())
            records->clear();

        return;
    }

    if (stage == FrameStage::NET_UPDATE_END) {
        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive()
                || !entity->isEnemy())
            {
                if (records[i].size() > 2)
                    records[i].clear();

                continue;
            }

            auto& rec = records[i];
            for (auto r = rec.begin(); r != rec.end();) {
                if (!valid(r->simulationTime))
                    r = rec.erase(r);
                else
                    r++;
            }

            if (records[i].size() && (records[i].front().simulationTime == entity->getProperty<float>("m_flSimulationTime")))
                continue;

            while (records[i].size() > 3 && records[i].size() > static_cast<size_t>(timeToTicks(static_cast<float>(config.backtrack.timeLimit) / 1000.f)))
                records[i].pop_back();

            auto varmap = reinterpret_cast<uintptr_t>(entity) + 0x24;
            auto varscount = *reinterpret_cast<int*>(static_cast<uintptr_t>(varmap) + 0x14);
            for (int j = 0; j < varscount; j++)
                *reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(varmap) + j * 0xC) = 0;

            Record record{ };
            record.mins = entity->getProperty<Vector>("m_vecMins");
            record.maxs = entity->getProperty<Vector>("m_vecMaxs");
            record.head = entity->getBonePosition(8);
            record.flags = entity->getProperty<int>("m_fFlags");
            record.simulationTime = entity->getProperty<float>("m_flSimulationTime");

            entity->setupBones(record.matrix, 128, 0x7FF00, memory.globalVars->currenttime);

            records[i].emplace_front(record);
        }
    }
}

void Backtrack::run(UserCmd* cmd) noexcept
{
    if (!config.backtrack.enabled)
        return;

    if (!(cmd->buttons & UserCmd::IN_ATTACK))
        return;

    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!localPlayer)
        return;

    auto localPlayerEyePosition = localPlayer->getEyePosition();

    auto bestFov{ 255.f };
    Entity * bestTarget{ };
    int bestTargetIndex{ };
    Vector bestTargetHeadPosition{ };
    int bestRecord{ };

    for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getEntity(i);
        if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive()
            || !entity->isEnemy())
            continue;

        auto headPosition = entity->getBonePosition(8);

        auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, headPosition, cmd->viewangles);
        auto fov = std::hypotf(angle.x, angle.y);
        if (fov < bestFov) {
            bestFov = fov;
            bestTarget = entity;
            bestTargetIndex = i;
            bestTargetHeadPosition = headPosition;
        }
    }

    if (bestTarget) {
        if (records[bestTargetIndex].size() <= 3 || (config.backtrack.ignoreSmoke && memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTargetHeadPosition, 1)))
            return;

        bestFov = 255.f;

        for (size_t i = 0; i < records[bestTargetIndex].size(); i++) {
            auto record = &records[bestTargetIndex][i];
            if (!record || !valid(record->simulationTime))
                continue;

            auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, record->head, cmd->viewangles);
            auto fov = std::hypotf(angle.x, angle.y);
            if (fov < bestFov) {
                bestFov = fov;
                bestRecord = i;
            }
        }
    }

    if (bestRecord) {
        auto record = records[bestTargetIndex][bestRecord];

        bestTarget->setProperty<Vector>("m_vecMins", record.mins);
        bestTarget->setProperty<Vector>("m_vecMaxs", record.maxs);
        if (bestTarget->getProperty<int>("m_fFlags") & 1)
            bestTarget->setProperty<int>("m_fFlags", record.flags);

        cmd->tick_count = timeToTicks(record.simulationTime);
    }
}