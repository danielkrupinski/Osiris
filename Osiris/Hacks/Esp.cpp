#include "Esp.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/Vector.h"

static bool worldToScreen(const Vector& in, Vector& out) noexcept
{
    static auto matrix = interfaces.engine->worldToScreenMatrix();
    float w = matrix[3][0] * in.x + matrix[3][1] * in.y + matrix[3][2] * in.z + matrix[3][3];

    if (w > 0.001f) {
        const auto [width, height] = interfaces.surface->getScreenSize();
        out.x = width / 2 * (1 + (matrix[0][0] * in.x + matrix[0][1] * in.y + matrix[0][2] * in.z + matrix[0][3]) / w);
        out.y = height / 2 * (1 - (matrix[1][0] * in.x + matrix[1][1] * in.y + matrix[1][2] * in.z + matrix[1][3]) / w);
        out.z = 0.0f;
        return true;
    }
    return false;
}

void Esp::render() noexcept
{
    if (interfaces.engine->isInGame()) {
        const auto [width, height] = interfaces.surface->getScreenSize();
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity == localPlayer || entity->isDormant()
                || !entity->isAlive())
                continue;
            Vector position;
            if (worldToScreen(entity->getProperty<Vector>("m_vecOrigin"), position)) {
                if (!entity->isEnemy()) {
                    if (entity->isVisible()) {
                        if (config.esp[0].enabled) {
                            if (config.esp[0].snaplines) {
                                interfaces.surface->setDrawColor(config.esp[0].snaplinesColor, 255);
                                interfaces.surface->drawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));
                            }
                        }
                    } else {
                        if (config.esp[1].enabled) {
                            if (config.esp[1].snaplines) {
                                interfaces.surface->setDrawColor(config.esp[1].snaplinesColor, 255);
                                interfaces.surface->drawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));
                            }
                        }
                    }
                    
                } else {
                    if (entity->isVisible()) {
                        if (config.esp[2].enabled) {
                            if (config.esp[2].snaplines) {
                                interfaces.surface->setDrawColor(config.esp[2].snaplinesColor, 255);
                                interfaces.surface->drawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));
                            }
                        }
                    } else {
                        if (config.esp[3].enabled) {
                            if (config.esp[3].snaplines) {
                                interfaces.surface->setDrawColor(config.esp[3].snaplinesColor, 255);
                                interfaces.surface->drawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));
                            }
                        }
                    }
                }
            }
        }
    }
}
