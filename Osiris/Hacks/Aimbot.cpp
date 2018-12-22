#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

#include "Aimbot.h"

static int findTarget(UserCmd* cmd, Vector& position)
{
    for (int i = 1; i < interfaces.engine->getMaxClients(); i++) {
        auto entity = interfaces.entityList->getClientEntity(i);
        if (!entity || !entity->isDormant() || !entity->isAlive() || !entity->isEnemy())
            continue;
    }
}

void Aimbot::run()
{

}
