#include "Backtrack.h"
#include "Aimbot.h"
#include "../Config.h"
#include "../SDK/Entity.h"
#include "../SDK/FrameStage.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/UserCmd.h"

std::deque<Backtrack::Record> Backtrack::records[65];
std::deque<Backtrack::Record> Backtrack::invalid_record[2];

std::deque<Backtrack::IncomingSequence>Backtrack::sequences;
Backtrack::Cvars Backtrack::cvars;

void Backtrack::update(FrameStage stage) noexcept
{
    if (!config->backtrack.enabled || !localPlayer || !localPlayer->isAlive()) {
        for (auto& record : records)
            record.clear();

        return;
    }

    if (stage == FrameStage::RENDER_START) {

        Record record_inv{};
        Record invalid{};
        invalid.invalid = true;
        invalid.wasTargeted = false;
        invalid_record[0].push_front(invalid);
        invalid_record[0].push_front(invalid);
    	
        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            auto entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get())) {
                records[i].clear();
                continue;
            }

            if (!records[i].empty() && (records[i].front().simulationTime == entity->simulationTime()))
                continue;

            Record record{ };
            record.origin = entity->getAbsOrigin();
        	record.head = entity->getBonePosition(8);
            record.simulationTime = entity->simulationTime();
        	record.PreviousAct = -1;
        	auto AnimState = entity->getAnimstate();

        	if (AnimState) {
                for (int b = 0; b < entity->getAnimationLayerCount(); b++) {
                    auto AnimLayer = entity->getAnimationLayer(b);
                    int currAct = entity->getSequenceActivity(AnimLayer->sequence);
                    if ((currAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST) || (currAct == 980)) {
                        record.PreviousAct = currAct;
                        break;
                    }
                    else{
                        record.PreviousAct = currAct;
                    }
                }
            }

        	if (!(!records[i].empty() && records[i].size() && Backtrack::valid(records[i].front().simulationTime))) {
                record.missedshots = 0;
                record.prevhealth = entity->health();
                record.wasTargeted = false;
                record.wasUpdated = false;
            }
            else {
                record.missedshots = records[i].front().missedshots;
                record.prevhealth = entity->health();
                record.wasTargeted = records[i].front().wasTargeted;
                record.wasUpdated = records[i].front().wasUpdated;
            }

            entity->setupBones(record.matrix, 256, 0x7FF00, memory->globalVars->currenttime);

            records[i].push_front(record);

        	int timeLimit = config->backtrack.timeLimit; if (timeLimit > 200) timeLimit = 200;
            while (records[i].size() > 3 && records[i].size() > static_cast<size_t>(timeToTicks(static_cast<float>(config->backtrack.timeLimit) / 1000.f + getExtraTicks())))
                records[i].pop_back();

            if (auto invalid = std::find_if(std::cbegin(records[i]), std::cend(records[i]), [](const Record & rec) { return !valid(rec.simulationTime); }); invalid != std::cend(records[i]))
                records[i].erase(invalid, std::cend(records[i]));
        }
    }
}

void Backtrack::run(UserCmd* cmd) noexcept
{
    if (!config->backtrack.enabled)
        return;

    if (!(cmd->buttons & UserCmd::IN_ATTACK))
		return;
	
    if (!localPlayer)
        return;

	if (config->backtrack.onKey) {
        if (GetAsyncKeyState(config->backtrack.fakeLatencyKey))
            config->backtrack.fakeLatency = !(config->backtrack.fakeLatency);
            if (config->backtrack.fakeLatency && (config->backtrack.timeLimit < 200)) {
                config->backtrack.timeLimit += 200;
            }
            else if (!(config->backtrack.fakeLatency) &&  config->backtrack.timeLimit > 200) {
                config->backtrack.timeLimit -= 200;
            }
    }

    auto localPlayerEyePosition = localPlayer->getEyePosition();

    auto bestFov{ 255.f };
    Entity * bestTarget{ };
    int bestTargetIndex{ };
    Vector bestTargetOrigin{ };
	Vector bestTargetHead{ };
    int bestRecord{ };

    const auto aimPunch = localPlayer->getAimPunch();

    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()
            || !entity->isOtherEnemy(localPlayer.get()))
            continue;

        auto origin = entity->getAbsOrigin();
    	auto head = entity->getBonePosition(8);
    	

        auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, head, cmd->viewangles + (config->backtrack.recoilBasedFov ? aimPunch : Vector{ }));
        auto fov = std::hypotf(angle.x, angle.y);
        if (fov < bestFov) {
            bestFov = fov;
            bestTarget = entity;
            bestTargetIndex = i;
            bestTargetOrigin = origin;
        }
    }

    if (bestTarget) {
        if (records[bestTargetIndex].size() <= 3 || (!config->backtrack.ignoreSmoke && memory->lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTargetHead, 1)))
            return;

        bestFov = 255.f;

        for (size_t i = 0; i < records[bestTargetIndex].size(); i++) {
            const auto& record = records[bestTargetIndex][i];
            if (!valid(record.simulationTime))
                continue;

            auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, record.head, cmd->viewangles + (config->backtrack.recoilBasedFov ? aimPunch : Vector{ }));
            auto fov = std::hypotf(angle.x, angle.y);
            if (fov < bestFov) {
                bestFov = fov;
                bestRecord = i;
            }
        }
    }

    if (bestRecord) {
        const auto& record = records[bestTargetIndex][bestRecord];
        memory->setAbsOrigin(bestTarget, record.origin);
        cmd->tickCount = timeToTicks(record.simulationTime + getLerp());
    }
}

void Backtrack::AddLatencyToNetwork(NetworkChannel* network, float latency) noexcept
{
    for (auto& sequence : sequences)
    {
        if (memory->globalVars->serverTime() - sequence.servertime >= latency)
        {
            network->InReliableState = sequence.inreliablestate;
            network->InSequenceNr = sequence.sequencenr;
            break;
        }
    }
}

void Backtrack::UpdateIncomingSequences(bool reset) noexcept
{
    static float lastIncomingSequenceNumber = 0.f;
    int timeLimit = config->backtrack.timeLimit; if (timeLimit <= 200) { timeLimit = 0; }
    else { timeLimit = 200; }
	
    if (!config->backtrack.fakeLatency || config->backtrack.timeLimit == 0)
        return;

    if (!localPlayer)
        return;

    auto network = interfaces->engine->getNetworkChannel();
    if (!network)
        return;

    if (network->InSequenceNr != lastIncomingSequenceNumber)
    {
        lastIncomingSequenceNumber = static_cast<float>(network->InSequenceNr);

        IncomingSequence sequence{ };
        sequence.inreliablestate = network->InReliableState;
        sequence.sequencenr = network->InSequenceNr;
        sequence.servertime = memory->globalVars->serverTime();
        sequences.push_front(sequence);
    }

    while (sequences.size() > 2048)
        sequences.pop_back();
}

int Backtrack::timeToTicks(float time) noexcept
{
    return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick);
}
