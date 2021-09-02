#include <algorithm>
#include <array>

#include "Aimbot.h"
#include "Backtrack.h"
#include "../ConfigStructs.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Cvar.h"
#include "../SDK/ConVar.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/UserCmd.h"
using namespace Backtrack;

#if OSIRIS_BACKTRACK()


//static std::array<std::deque<Backtrack::Record>, 65> records;
static std::deque<Backtrack::IncomingSequence>Backtrack::sequences;
static std::array<std::deque<Backtrack::Record>, 513> records;

struct Cvars {
    ConVar* updateRate;
    ConVar* maxUpdateRate;
    ConVar* interp;
    ConVar* interpRatio;
    ConVar* minInterpRatio;
    ConVar* maxInterpRatio;
    ConVar* maxUnlag;
};

static Cvars cvars;

static auto timeToTicks(float time) noexcept
{
    return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick);
}

void Backtrack::update(FrameStage stage) noexcept
{
    int timeLimit = backtrackConfig.timeLimit;
    if (timeLimit <= 0 || timeLimit >= 201) { backtrackConfig.fakeLatency = true; }
    else { backtrackConfig.fakeLatency = false; }
    if (timeLimit <= 0 || timeLimit > 400) { backtrackConfig.enabled = false; }
    else { backtrackConfig.enabled = true; }

    if (stage == FrameStage::RENDER_START)
    {
        if (!backtrackConfig.enabled || !localPlayer || !localPlayer->isAlive()) {
            for (auto& record : records)
                record.clear();
            return;
        }

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
            record.simulationTime = entity->simulationTime();

            entity->setupBones(record.matrix, 256, 0x7FF00, memory->globalVars->currenttime);

            records[i].push_front(record);

            while (records[i].size() > 3 && records[i].size() > static_cast<size_t>(timeToTicks(static_cast<float>(backtrackConfig.timeLimit) / 1000.f + getExtraTicks())))
                records[i].pop_back();

            if (auto invalid = std::find_if(std::cbegin(records[i]), std::cend(records[i]), [](const Record & rec) { return !valid(rec.simulationTime); }); invalid != std::cend(records[i]))
                records[i].erase(invalid, std::cend(records[i]));
        }
    }
}

static float getLerp() noexcept
{
    auto ratio = std::clamp(cvars.interpRatio->getFloat(), cvars.minInterpRatio->getFloat(), cvars.maxInterpRatio->getFloat());
    return (std::max)(cvars.interp->getFloat(), (ratio / ((cvars.maxUpdateRate) ? cvars.maxUpdateRate->getFloat() : cvars.updateRate->getFloat())));
}

void Backtrack::run(UserCmd* cmd) noexcept
{
    if (!backtrackConfig.enabled)
        return;

    if (!localPlayer)
        return;

    if (!(cmd->buttons & UserCmd::IN_ATTACK))
        return;

    auto localPlayerEyePosition = localPlayer->getEyePosition();

    auto bestFov{ 255.f };
    Entity * bestTarget{ };
    int bestTargetIndex{ };
    Vector bestTargetOrigin{ };
    int bestRecord{ };

    const auto aimPunch = localPlayer->getAimPunch();

    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get()))
            continue;

        const auto& origin = entity->getAbsOrigin();

        auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, origin, cmd->viewangles + (backtrackConfig.recoilBasedFov ? aimPunch : Vector{ }));
        auto fov = std::hypotf(angle.x, angle.y);
        if (fov < bestFov) {
            bestFov = fov;
            bestTarget = entity;
            bestTargetIndex = i;
            bestTargetOrigin = origin;
        }
    }

    if (bestTarget) {
        if (records[bestTargetIndex].size() <= 3 || (!backtrackConfig.ignoreSmoke && memory->lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTargetOrigin, 1)))
            return;

        bestFov = 255.f;

        for (size_t i = 0; i < records[bestTargetIndex].size(); i++) {
            const auto& record = records[bestTargetIndex][i];
            if (!valid(record.simulationTime))
                continue;

            auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, record.origin, cmd->viewangles + (backtrackConfig.recoilBasedFov ? aimPunch : Vector{ }));
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

const std::deque<Backtrack::Record>* Backtrack::getRecords(std::size_t index) noexcept
{
    if (!backtrackConfig.enabled)
        return nullptr;
    return &records[index];
}

bool Backtrack::valid(float simtime) noexcept
{
    const auto network = interfaces->engine->getNetworkChannel();
    if (!network)
        return false;

    auto delta = std::clamp(network->getLatency(0) + network->getLatency(1) + getLerp(), 0.f, cvars.maxUnlag->getFloat()) - (memory->globalVars->serverTime() - simtime);
    return std::abs(delta) <= 0.2f;
}
float Backtrack::getExtraTicks() noexcept
{
    auto network = interfaces->engine->getNetworkChannel();
    if (!network)
        return 0.f;
    return std::clamp(network->getLatency(1) - network->getLatency(0), 0.f, cvars.maxUnlag->getFloat());
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

    if (!backtrackConfig.enabled)
        return;

    if (backtrackConfig.timeLimit == 0)
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
void Backtrack::init() noexcept
{
    cvars.updateRate = interfaces->cvar->findVar("cl_updaterate");
    cvars.maxUpdateRate = interfaces->cvar->findVar("sv_maxupdaterate");
    cvars.interp = interfaces->cvar->findVar("cl_interp");
    cvars.interpRatio = interfaces->cvar->findVar("cl_interp_ratio");
    cvars.minInterpRatio = interfaces->cvar->findVar("sv_client_min_interp_ratio");
    cvars.maxInterpRatio = interfaces->cvar->findVar("sv_client_max_interp_ratio");
    cvars.maxUnlag = interfaces->cvar->findVar("sv_maxunlag");
}

static bool backtrackWindowOpen = false;

void Backtrack::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Backtrack")) {
        backtrackWindowOpen = true;
        ImGui::SetWindowFocus("Backtrack");
        ImGui::SetWindowPos("Backtrack", { 100.0f, 100.0f });
    }
}

void Backtrack::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Backtrack")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

void Backtrack::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!backtrackWindowOpen)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Backtrack", &backtrackWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }
    ImGui::PushID(0);
	ImGui::PushItemWidth(220.0f);
	ImGui::SliderInt("", &backtrackConfig.timeLimit, 0, 400, "Time limit %d ms");
	ImGui::PopItemWidth();
	ImGui::PopID();
	ImGui::Checkbox("Ignore smoke", &backtrackConfig.ignoreSmoke);
	ImGui::SameLine();
	ImGui::Checkbox("Recoil based fov", &backtrackConfig.recoilBasedFov);
	ImGui::Checkbox("Chams Last/All Tick", &backtrackConfig.drawAllChams);
    if (!contentOnly)
        ImGui::End();
}

static void to_json(json& j, const BacktrackConfig& o, const BacktrackConfig& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Ignore smoke", ignoreSmoke);
    WRITE("Recoil based fov", recoilBasedFov);
    WRITE("Time limit", timeLimit);
    WRITE("Fake Latency", fakeLatency);
    WRITE("Draw All Chams", drawAllChams);
}

json Backtrack::toJson() noexcept
{
    json j;
    to_json(j, backtrackConfig);
    return j;
}

static void from_json(const json& j, BacktrackConfig& b)
{
    read(j, "Enabled", b.enabled);
    read(j, "Ignore smoke", b.ignoreSmoke);
    read(j, "Recoil based fov", b.recoilBasedFov);
    read(j, "Time limit", b.timeLimit);
    read(j, "Fake Latency", b.fakeLatency);
    read(j, "Draw All Chams", b.drawAllChams);
}

void Backtrack::fromJson(const json& j) noexcept
{
    from_json(j, backtrackConfig);
}

void Backtrack::resetConfig() noexcept
{
    backtrackConfig = {};
}

#else

namespace Backtrack
{
    void update(FrameStage) noexcept {}
    void run(UserCmd*) noexcept {}

    const std::deque<Record>* getRecords(std::size_t index) noexcept { return nullptr; }
    bool valid(float simtime) noexcept { return false; }
    void init() noexcept {}

    // GUI
    void menuBarItem() noexcept {}
    void tabItem() noexcept {}
    void drawGUI(bool contentOnly) noexcept {}

    // Config
    json toJson() noexcept { return {}; }
    void fromJson(const json& j) noexcept {}
    void resetConfig() noexcept {}
}

#endif
