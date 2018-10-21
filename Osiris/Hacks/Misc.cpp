#include "../Config.h"
#include "../Memory.h"
#include "Misc.h"

void Misc::bunnyHop(UserCmd* cmd)
{
    if (config.misc.bunnyHop) {
        static auto jumped = false;
        static auto fake = false;
        if (!jumped && fake) {
            fake = false;
            cmd->buttons |= 2;
        }
        else if (cmd->buttons & 2) {
            if (*((*memory.localPlayer)->getFlags()) & 1) {
                jumped = true;
                fake = true;
            }
            else {
                cmd->buttons &= ~2;
                jumped = false;
            }
        }
        else {
            jumped = false;
            fake = false;
        }
    }
}

void Misc::disablePostProcessing()
{
    if (*memory.disablePostProcessing != config.misc.disablePostProcessing)
        *memory.disablePostProcessing = config.misc.disablePostProcessing;
}
