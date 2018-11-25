#include <string_view>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "Misc.h"

void Misc::bunnyHop(UserCmd* cmd) noexcept
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

void Misc::disablePostProcessing() noexcept
{
    if (*memory.disablePostProcessing != config.misc.disablePostProcessing)
        *memory.disablePostProcessing = config.misc.disablePostProcessing;
}

void Misc::skybox() noexcept
{
    if (config.misc.skybox != xorstr_("Default"))
        reinterpret_cast<void(__fastcall*)(const char*)>(memory.loadSky)(config.misc.skybox.c_str());
}

void Misc::clanTag() noexcept
{
    static std::string_view clanTag = config.misc.clanTag;
    if (clanTag != config.misc.clanTag) {
        reinterpret_cast<void(__fastcall*)(const char*, const char*)>(memory.setClanTag)(config.misc.clanTag.c_str(), "");
        clanTag = config.misc.clanTag;
    }
}

void Misc::colorWorld() noexcept
{
    static auto red = interfaces.cvar->FindVar(xorstr_("mat_ambient_light_r"));
    static auto green = interfaces.cvar->FindVar(xorstr_("mat_ambient_light_g"));
    static auto blue = interfaces.cvar->FindVar(xorstr_("mat_ambient_light_b"));

    red->SetValue(config.misc.worldColor[0]);
    green->SetValue(config.misc.worldColor[1]);
    blue->SetValue(config.misc.worldColor[2]);
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.cvar->FindVar(xorstr_("cl_ragdoll_gravity"));
    ragdollGravity->SetValue(config.misc.inverseRagdollGravity ? -600 : 600);
}

void Misc::reduceFlashEffect() noexcept
{
    *reinterpret_cast<float*>((*memory.localPlayer) + 0xA328) = 255.0f - config.misc.flashReduction * 2.55f;
}

void Misc::removeBlood() noexcept
{
    static auto enableBlood = interfaces.cvar->FindVar(xorstr_("violence_hblood"));
    enableBlood->SetValue(!config.misc.noBlood);
}

void Misc::removeSmoke() noexcept
{
    static const std::string smokeMaterials[]{
        xorstr_("particle/vistasmokev1/vistasmokev1_emods"),
        xorstr_("particle/vistasmokev1/vistasmokev1_emods_impactdust"),
        xorstr_("particle/vistasmokev1/vistasmokev1_fire"),
        xorstr_("particle/vistasmokev1/vistasmokev1_smokegrenade"),
    };

    for (const auto& mat : smokeMaterials) {
        Material* material = interfaces.materialSystem->findMaterial(mat.c_str(), xorstr_("Other textures"));
        material->setMaterialVarFlag(MaterialVar::NO_DRAW, config.misc.noSmoke);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.misc.wireframeSmoke);
    }

    if (config.misc.noSmoke || config.misc.wireframeSmoke)
        *memory.smokeCount = 0;
}
