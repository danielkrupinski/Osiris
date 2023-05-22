#include <algorithm>
#include <array>

#include "Aimbot.h"
#include "Backtrack.h"
#include "../ConfigStructs.h"
#include "../Memory.h"
#include <CSGO/Cvar.h>
#include <CSGO/ConVar.h>
#include <CSGO/Engine.h>
#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/Constants/ConVarNames.h>
#include <CSGO/Constants/FrameStage.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/NetworkChannel.h>
#include <CSGO/UserCmd.h>

#include <Interfaces/ClientInterfaces.h>
#include <Interfaces/OtherInterfaces.h>
#include <Config/ResetConfigurator.h>

static auto timeToTicks(const Memory& memory, float time) noexcept
{
    return static_cast<int>(0.5f + time / memory.globalVars->intervalPerTick);
}

Backtrack::Backtrack(const csgo::Cvar& cvar) : cvars{
        .updateRate = csgo::ConVar::from(retSpoofGadgets->client, cvar.findVar(csgo::cl_updaterate)),
        .maxUpdateRate = csgo::ConVar::from(retSpoofGadgets->client, cvar.findVar(csgo::sv_maxupdaterate)),
        .interp = csgo::ConVar::from(retSpoofGadgets->client, cvar.findVar(csgo::cl_interp)),
        .interpRatio = csgo::ConVar::from(retSpoofGadgets->client, cvar.findVar(csgo::cl_interp_ratio)),
        .minInterpRatio = csgo::ConVar::from(retSpoofGadgets->client, cvar.findVar(csgo::sv_client_min_interp_ratio)),
        .maxInterpRatio = csgo::ConVar::from(retSpoofGadgets->client, cvar.findVar(csgo::sv_client_max_interp_ratio)),
        .maxUnlag = csgo::ConVar::from(retSpoofGadgets->client, cvar.findVar(csgo::sv_maxunlag)) }
{
    ResetConfigurator configurator;
    configure(configurator);
}

void Backtrack::update(const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, csgo::FrameStage stage) noexcept
{
    if (stage == csgo::FrameStage::RENDER_START) {
        if (!enabled || !localPlayer || !localPlayer.get().isAlive()) {
            for (auto& record : records)
                record.clear();
            return;
        }

        for (int i = 1; i <= engineInterfaces.getEngine().getMaxClients(); i++) {
            const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(i));
            if (entity.getPOD() == nullptr || entity.getPOD() == localPlayer.get().getPOD() || entity.getNetworkable().isDormant() || !entity.isAlive() || !entity.isOtherEnemy(memory, localPlayer.get())) {
                records[i].clear();
                continue;
            }

            if (!records[i].empty() && (records[i].front().simulationTime == entity.simulationTime()))
                continue;

            Record record{ };
            record.origin = entity.getAbsOrigin();
            record.simulationTime = entity.simulationTime();

            entity.setupBones(record.matrix, 256, 0x7FF00, memory.globalVars->currenttime);

            records[i].push_front(record);

            while (records[i].size() > 3 && records[i].size() > static_cast<size_t>(timeToTicks(memory, static_cast<float>(timeLimit) / 1000.f)))
                records[i].pop_back();

            if (auto invalid = std::find_if(std::cbegin(records[i]), std::cend(records[i]), [&memory, &engineInterfaces, this](const Record & rec) { return !valid(engineInterfaces.getEngine(), memory, rec.simulationTime); }); invalid != std::cend(records[i]))
                records[i].erase(invalid, std::cend(records[i]));
        }
    }
}

float Backtrack::getLerp() noexcept
{
    auto ratio = std::clamp(cvars.interpRatio.getFloat(), cvars.minInterpRatio.getFloat(), cvars.maxInterpRatio.getFloat());
    return (std::max)(cvars.interp.getFloat(), (ratio / ((cvars.maxUpdateRate.getPOD() != nullptr) ? cvars.maxUpdateRate.getFloat() : cvars.updateRate.getFloat())));
}

void Backtrack::run(const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory, csgo::UserCmd* cmd) noexcept
{
    if (!enabled)
        return;

    if (!(cmd->buttons & csgo::UserCmd::IN_ATTACK))
        return;

    if (!localPlayer)
        return;

    auto localPlayerEyePosition = localPlayer.get().getEyePosition();

    auto bestFov{ 255.f };
    csgo::EntityPOD* bestTarget{ };
    int bestTargetIndex{ };
    csgo::Vector bestTargetOrigin{ };
    int bestRecord{ };

    const auto aimPunch = localPlayer.get().getAimPunch();

    for (int i = 1; i <= engineInterfaces.getEngine().getMaxClients(); i++) {
        const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(i));
        if (entity.getPOD() == nullptr || entity.getPOD() == localPlayer.get().getPOD() || entity.getNetworkable().isDormant() || !entity.isAlive()
            || !entity.isOtherEnemy(memory, localPlayer.get()))
            continue;

        const auto& origin = entity.getAbsOrigin();

        auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, origin, cmd->viewangles + (recoilBasedFov ? aimPunch : csgo::Vector{ }));
        auto fov = std::hypotf(angle.x, angle.y);
        if (fov < bestFov) {
            bestFov = fov;
            bestTarget = entity.getPOD();
            bestTargetIndex = i;
            bestTargetOrigin = origin;
        }
    }

    if (bestTarget) {
        if (records[bestTargetIndex].size() <= 3 || (!ignoreSmoke && memory.lineGoesThroughSmoke(localPlayer.get().getEyePosition(), bestTargetOrigin, 1)))
            return;

        bestFov = 255.f;

        for (size_t i = 0; i < records[bestTargetIndex].size(); i++) {
            const auto& record = records[bestTargetIndex][i];
            if (!valid(engineInterfaces.getEngine(), memory, record.simulationTime))
                continue;

            auto angle = Aimbot::calculateRelativeAngle(localPlayerEyePosition, record.origin, cmd->viewangles + (recoilBasedFov ? aimPunch : csgo::Vector{ }));
            auto fov = std::hypotf(angle.x, angle.y);
            if (fov < bestFov) {
                bestFov = fov;
                bestRecord = i;
            }
        }
    }

    if (bestRecord) {
        const auto& record = records[bestTargetIndex][bestRecord];
        memory.setAbsOrigin(bestTarget, record.origin);
        cmd->tickCount = timeToTicks(memory, record.simulationTime + getLerp());
    }
}

std::deque<Backtrack::Record>* Backtrack::getRecords(std::size_t index) noexcept
{
    if (!enabled)
        return nullptr;
    return &records[index];
}

bool Backtrack::valid(const csgo::Engine& engine, const Memory& memory, float simtime) noexcept
{
    const auto network = engine.getNetworkChannel();
    if (!network)
        return false;

    auto delta = std::clamp(csgo::NetworkChannel::from(retSpoofGadgets->client, network).getLatency(0) + csgo::NetworkChannel::from(retSpoofGadgets->client, network).getLatency(1) + getLerp(), 0.f, cvars.maxUnlag.getFloat()) - (memory.globalVars->serverTime() - simtime);
    return std::abs(delta) <= 0.2f;
}

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
    ImGui::Checkbox("Enabled", &enabled);
    ImGui::Checkbox("Ignore smoke", &ignoreSmoke);
    ImGui::Checkbox("Recoil based fov", &recoilBasedFov);
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("Time limit", &timeLimit, 1, 200, "%d ms");
    ImGui::PopItemWidth();
    if (!contentOnly)
        ImGui::End();
}
