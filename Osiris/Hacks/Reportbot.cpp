#include <vector>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/GlobalVars.h"

#include "Reportbot.h"

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

    for (int i = 1; i <= interfaces.engine->getMaxClients(); ++i) {
        const auto entity = interfaces.entityList->getEntity(i);

        if (!entity || entity == localPlayer)
            continue;

        if (config.reportbot.target != 2 && (entity->isEnemy() ? config.reportbot.target != 0 : config.reportbot.target != 1))
            continue;

        if (PlayerInfo playerInfo; interfaces.engine->getPlayerInfo(i, playerInfo)) {
            static std::vector<__int64> reportedPlayers;
            if (playerInfo.steamID64 == 0 || std::find(reportedPlayers.cbegin(), reportedPlayers.cend(), playerInfo.steamID64) != reportedPlayers.cend())
                continue;

            std::string report;
            if (config.reportbot.aimbot)
                report += "aimbot,";
            if (config.reportbot.wallhack)
                report += "wallhack,";
            if (config.reportbot.other)
                report += "speedhack,";
            if (config.reportbot.griefing)
                report += "grief,";
            if (config.reportbot.voiceAbuse)
                report += "voiceabuse,";
            if (config.reportbot.textAbuse)
                report += "textabuse,";

            if (!report.empty()) {
                memory.submitReport(std::to_string(playerInfo.steamID64).c_str(), report.c_str());
                reportedPlayers.push_back(playerInfo.steamID64);
            }
            break;
        }
    }
    lastReportTime = memory.globalVars->realtime;
}
