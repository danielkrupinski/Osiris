#pragma once

namespace csgo
{

#define CONVAR_NAME(name) constexpr auto name = #name

CONVAR_NAME(cl_updaterate);
CONVAR_NAME(sv_maxupdaterate);
CONVAR_NAME(cl_interp);
CONVAR_NAME(cl_interp_ratio);
CONVAR_NAME(sv_client_min_interp_ratio);
CONVAR_NAME(sv_client_max_interp_ratio);
CONVAR_NAME(sv_maxunlag);
CONVAR_NAME(mp_plant_c4_anywhere);
CONVAR_NAME(name);
CONVAR_NAME(cl_grenadepreview);
CONVAR_NAME(mp_buytime);
CONVAR_NAME(cl_righthand);
CONVAR_NAME(cl_ragdoll_gravity);
CONVAR_NAME(mat_force_tonemap_scale);
CONVAR_NAME(r_3dsky);
CONVAR_NAME(cl_csm_enabled);
CONVAR_NAME(sv_skyname);
CONVAR_NAME(cl_fullupdate);
CONVAR_NAME(sv_cheats);

#undef CONVAR_NAME

constexpr auto panorama_disable_blur = "@panorama_disable_blur";

}
