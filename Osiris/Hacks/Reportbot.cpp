#include <vector>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/GlobalVars.h"

#include "Reportbot.h"

void Reportbot::run()
{
    if (config.reportbot.enabled) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        static auto lastReportTime{ 0.0f };

        if (lastReportTime + config.reportbot.delay <= memory.globalVars->realtime) {
            for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
                if (auto entity = interfaces.entityList->getEntity(i); entity && entity != localPlayer && (config.reportbot.target == 2 || (config.reportbot.target ? !entity->isEnemy() : entity->isEnemy()))) {
                    static PlayerInfo playerInfo;
                    static std::vector<__int64> reportedPlayers;
                    if (interfaces.engine->getPlayerInfo(entity->index(), playerInfo) && std::find(std::begin(reportedPlayers), std::end(reportedPlayers), playerInfo.steamID64) == std::end(reportedPlayers)) {
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
            }
            lastReportTime = memory.globalVars->realtime;
        }
    }
}
