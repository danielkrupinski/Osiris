#include "Backtrack.h"
#include "Aimbot.h"
#include "Chams.h"
#include "../Config.h"
#include "../SDK/FrameStage.h"

std::deque<Backtrack::Record> Backtrack::records[65];
Backtrack::Cvars Backtrack::cvars;

void Backtrack::update(FrameStage stage) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!config.backtrack.enabled || !localPlayer || !localPlayer->isAlive()) {
        for (auto& record : records)
            record.clear();

        return;
    }

    if (stage == FrameStage::RENDER_START) {
        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive() || !entity->isEnemy()) {
                records[i].clear();
                continue;
            }

            if (!records[i].empty() && (records[i].front().simulationTime == entity->getProperty<float>("m_flSimulationTime")))
                continue;

            Record record{ };
            record.origin = entity->getAbsOrigin();
            record.simulationTime = entity->getProperty<float>("m_flSimulationTime");

            entity->setupBones(record.matrix, 128, 0x7FF00, memory.globalVars->currenttime);

            records[i].push_front(record);

            while (records[i].size() > 3 && records[i].size() > static_cast<size_t>(timeToTicks(static_cast<float>(config.backtrack.timeLimit) / 1000.f)))
                records[i].pop_back();

            if (auto invalid = std::find_if(std::cbegin(records[i]), std::cend(records[i]), [](const Record & rec) { return !valid(rec.simulationTime); }); invalid != std::cend(records[i]))
                records[i].erase(invalid, std::cend(records[i]));
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
    Vector bestTargetOrigin{ };
    int bestRecord{ };

    for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getEntity(i);
        if (!entity || entity == localPlayer || entity->isDormant() || !entity->isAlive()
            || !entity->isEnemy())
            continue;

        auto origin = entity->getAbsOrigin();

        auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, origin, cmd->viewangles);
        auto fov = std::hypotf(angle.x, angle.y);
        if (fov < bestFov) {
            bestFov = fov;
            bestTarget = entity;
            bestTargetIndex = i;
            bestTargetOrigin = origin;
        }
    }

    if (bestTarget) {
        if (records[bestTargetIndex].size() <= 3 || (!config.backtrack.ignoreSmoke && memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTargetOrigin, 1)))
            return;

        bestFov = 255.f;

        for (size_t i = 0; i < records[bestTargetIndex].size(); i++) {
            auto& record = records[bestTargetIndex][i];
            if (!valid(record.simulationTime))
                continue;

            auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, record.origin, cmd->viewangles);
            auto fov = std::hypotf(angle.x, angle.y);
            if (fov < bestFov) {
                bestFov = fov;
                bestRecord = i;
            }
        }
    }

    if (bestRecord) {
        auto record = records[bestTargetIndex][bestRecord];
        memory.setAbsOrigin(bestTarget, record.origin);
        cmd->tick_count = timeToTicks(record.simulationTime + getLerp());
    }
}