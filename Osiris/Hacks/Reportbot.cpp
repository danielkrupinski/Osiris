#include <vector>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/GlobalVars.h"

#include "Reportbot.h"

static std::vector<std::uint64_t> reportedPlayers;
static int currentRound;

void Reportbot::run() noexcept
{
    if (!config.reportbot.enabled)
        return;

    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!localPlayer)
        return;

    static auto lastReportTime = 0.0f;

    if (lastReportTime + config.reportbot.delay > memory.globalVars->realtime)
        return;

    if (currentRound >= config.reportbot.rounds)
        return;

    bool allPlayersReported = true;
    for (int i = 1; i <= interfaces.engine->getMaxClients(); ++i) {
        const auto entity = interfaces.entityList->getEntity(i);

        if (!entity || entity == localPlayer)
            continue;

        if (config.reportbot.target != 2 && (entity->isEnemy() ? config.reportbot.target != 0 : config.reportbot.target != 1))
            continue;

        PlayerInfo playerInfo;  
        if (!interfaces.engine->getPlayerInfo(i, playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::find(reportedPlayers.cbegin(), reportedPlayers.cend(), playerInfo.xuid) != reportedPlayers.cend())
            continue;

        std::string report;

        if (config.reportbot.textAbuse)
            report += "textabuse,";
        if (config.reportbot.griefing)
            report += "grief,";
        if (config.reportbot.wallhack)
            report += "wallhack,";
        if (config.reportbot.aimbot)
            report += "aimbot,";
        if (config.reportbot.other)
            report += "speedhack,";

        if (!report.empty()) {
            memory.submitReport(std::to_string(playerInfo.xuid).c_str(), report.c_str());
            lastReportTime = memory.globalVars->realtime;
            reportedPlayers.push_back(playerInfo.xuid);
        }
        allPlayersReported = false;
        break;
    }

    if (allPlayersReported) {
        reportedPlayers.clear();
        ++currentRound;
    }
}

void Reportbot::reset() noexcept
{
    currentRound = 0;
    reportedPlayers.clear();
}
