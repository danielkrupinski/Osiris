#include "../Config.h"
#include "../Memory.h"
#include "Visuals.h"

void Visuals::thirdperson()
{
    static bool isInThirdperson{ true };
    static float lastTime{ 0.0f };

    if (GetAsyncKeyState(config.visuals.thirdpersonKey) && memory.globalVars->realtime - lastTime > 0.5f) {
        isInThirdperson = !isInThirdperson;
        lastTime = memory.globalVars->realtime;
    }
    if (memory.input->isCameraInThirdPerson =
        (config.visuals.thirdperson && isInThirdperson
            && interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isAlive()))
        memory.input->cameraOffset.z = static_cast<float>(config.visuals.thirdpersonDistance);
}
