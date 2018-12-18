#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"

#include "Aimbot.h"

static int findTarget(UserCmd* cmd, Vector& position)
{
    for (int i = 1; i < interfaces.engineClient->getMaxClients(); i++) {
        auto entity = interfaces.clientEntityList->getClientEntity(i);
        if (!entity || !entity->isDormant() || !entity->isAlive_2() || !entity->isEnemy())
            continue;
    }
}

void Aimbot::run()
{

}
