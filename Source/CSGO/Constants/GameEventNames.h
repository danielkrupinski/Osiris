#pragma once

namespace csgo
{

#define GAMEEVENT_NAME(name) constexpr auto name = #name

GAMEEVENT_NAME(round_start);
GAMEEVENT_NAME(round_freeze_end);
GAMEEVENT_NAME(player_hurt);
GAMEEVENT_NAME(player_death);
GAMEEVENT_NAME(vote_cast);
GAMEEVENT_NAME(round_mvp);
GAMEEVENT_NAME(item_purchase);
GAMEEVENT_NAME(bullet_impact);

#undef GAMEEVENT_NAME

}
