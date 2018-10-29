#include "../Config.h"
#include "../Memory.h"
#include "Misc.h"

void Misc::bunnyHop(UserCmd* cmd)
{
    if (config.misc.bunnyHop) {
        constexpr int ON_GROUND{ 1 };
        constexpr int IN_JUMP{ 2 };

        static auto jumped = false;
        static auto fake = false;
        if (!jumped && fake) {
            fake = false;
            cmd->buttons |= IN_JUMP;
        }
        else if (cmd->buttons & IN_JUMP) {
            if (*((*memory.localPlayer)->getFlags()) & ON_GROUND) {
                jumped = true;
                fake = true;
            }
            else {
                cmd->buttons &= ~IN_JUMP;
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

void Misc::skybox()
{
    if (config.misc.skybox != xorstr_("Default"))
        reinterpret_cast<void (__fastcall*)(const char*)>(memory.loadSky)(config.misc.skybox.c_str());
}

void Misc::clanTag()
{

}
