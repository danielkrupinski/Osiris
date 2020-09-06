/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "item_definitions.hpp"
// These are std::vectors because else I'd have to write their size in the header or write my own container

#include <array>
#include <tuple>

// We need these for overriding viewmodels and icons
const game_data::weapon_info* game_data::get_weapon_info(int defindex)
{
    static constexpr std::array<std::pair<int, weapon_info>, 30> info{ {
        { WEAPON_KNIFE, { "models/weapons/v_knife_default_ct.mdl", "knife" } },
        { WEAPON_KNIFE_T,{"models/weapons/v_knife_default_t.mdl", "knife_t"} },
        { WEAPON_KNIFE_BAYONET,{"models/weapons/v_knife_bayonet.mdl", "bayonet"} },
        { WEAPON_KNIFE_CSS,{"models/weapons/v_knife_css.mdl", "knife_css"} },
        { WEAPON_KNIFE_SKELETON,{"models/weapons/v_knife_skeleton.mdl", "knife_skeleton"} },
        { WEAPON_KNIFE_OUTDOOR,{"models/weapons/v_knife_outdoor.mdl", "knife_outdoor"} },
        { WEAPON_KNIFE_CORD,{"models/weapons/v_knife_cord.mdl", "knife_cord"} },
        { WEAPON_KNIFE_CANIS,{"models/weapons/v_knife_canis.mdl", "knife_canis"} },
        { WEAPON_KNIFE_FLIP,{"models/weapons/v_knife_flip.mdl", "knife_flip"} },
        { WEAPON_KNIFE_GUT,{"models/weapons/v_knife_gut.mdl", "knife_gut"} },
        { WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "knife_karambit"} },
        { WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet"} },
        { WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "knife_tactical"} },
        { WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion"} },
        { WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie"} },
        { WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "knife_butterfly"} },
        { WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "knife_push"} },
        { WEAPON_KNIFE_URSUS,{"models/weapons/v_knife_ursus.mdl", "knife_ursus"} },
        { WEAPON_KNIFE_GYPSY_JACKKNIFE,{"models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife"} },
        { WEAPON_KNIFE_STILETTO,{"models/weapons/v_knife_stiletto.mdl", "knife_stiletto"} },
        { WEAPON_KNIFE_WIDOWMAKER,{"models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker"} },

        { GLOVE_STUDDED_BLOODHOUND,{"models/weapons/w_models/arms/w_glove_bloodhound.mdl"} },
        { GLOVE_T_SIDE,{"models/weapons/w_models/arms/w_glove_fingerless.mdl"} },
        { GLOVE_CT_SIDE,{"models/weapons/w_models/arms/w_glove_hardknuckle.mdl"} },
        { GLOVE_SPORTY,{"models/weapons/w_models/arms/w_glove_sporty.mdl"} },
        { GLOVE_SLICK,{"models/weapons/w_models/arms/w_glove_slick.mdl"} },
        { GLOVE_LEATHER_WRAP,{"models/weapons/w_models/arms/w_glove_handwrap_leathery.mdl"} },
        { GLOVE_MOTORCYCLE,{"models/weapons/w_models/arms/w_glove_motorcycle.mdl"} },
        { GLOVE_SPECIALIST,{"models/weapons/w_models/arms/w_glove_specialist.mdl"} },
        { GLOVE_HYDRA,{"models/weapons/w_models/arms/w_glove_bloodhound_hydra.mdl"} }
    } };

    const auto entry = std::find_if(info.cbegin(), info.cend(), [defindex](const auto& p) { return p.first == defindex; });
    return entry == info.cend() ? nullptr : &entry->second;
}
