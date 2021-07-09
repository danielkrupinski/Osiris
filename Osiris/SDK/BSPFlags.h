#pragma once

enum Contents
{
	contents_empty = 0x0,
	contents_solid = 0x1,
	contents_window = 0x2,
	contents_aux = 0x4,
	contents_grate = 0x8,
	contents_slime = 0x10,
	contents_water = 0x20,
	contents_blocklos = 0x40,
	contents_opaque = 0x80,
	contents_testfogvolume = 0x100,
	contents_unused = 0x200,
	contents_blocklight = 0x400,
	contents_team1 = 0x800,
	contents_team2 = 0x1000,
	contents_ignore_nodraw_opaque = 0x2000,
	contents_moveable = 0x4000,
	contents_areaportal = 0x8000,
	contents_playerclip = 0x10000,
	contents_monsterclip = 0x20000,
	contents_current0 = 0x40000,
	contents_current90 = 0x80000,
	contents_current180 = 0x100000,
	contents_current270 = 0x200000,
	contents_current_up = 0x400000,
	contents_current_down = 0x800000,
	contents_origin = 0x1000000,
	contents_monster = 0x2000000,
	contents_debris = 0x4000000,
	contents_detail = 0x8000000,
	contents_translucent = 0x10000000,
	contents_ladder = 0x20000000,
	contents_hitbox = 0x40000000,

	last_visible_contents = contents_opaque,
	all_visible_contents = last_visible_contents | last_visible_contents - 1
};

enum surf
{
	surf_light = 0x1,
	surf_sky2d = 0x2,
	surf_sky = 0x4,
	surf_warp = 0x8,
	surf_trans = 0x10,
	surf_noportal = 0x20,
	surf_trigger = 0x40,
	surf_nodraw = 0x80,
	surf_hint = 0x100,
	surf_skip = 0x200,
	surf_nolight = 0x400,
	surf_bumplight = 0x800,
	surf_noshadows = 0x1000,
	surf_nodecals = 0x2000,
	surf_nopaint = surf_nodecals,
	surf_nochop = 0x4000,
	surf_hitbox = 0x8000
};

enum mask
{
	mask_all = 0xFFFFFFFF,
	mask_solid = (contents_solid | contents_moveable | contents_window | contents_monster | contents_grate),
	mask_playersolid = (contents_solid | contents_moveable | contents_playerclip | contents_window | contents_monster | contents_grate),
	mask_npcsolid = (contents_solid | contents_moveable | contents_monsterclip | contents_window | contents_monster | contents_grate),
	mask_npcfluid = (contents_solid | contents_moveable | contents_monsterclip | contents_window | contents_monster),
	mask_water = (contents_water | contents_moveable | contents_slime),
	mask_opaque = (contents_water | contents_moveable | contents_opaque),
	mask_opaque_npc = (mask_opaque | contents_monster),
	mask_blocklos = (contents_solid | contents_moveable | contents_slime),
	mask_blocklos_npc = (mask_blocklos | contents_monster),
	mask_visible = (mask_opaque | contents_ignore_nodraw_opaque),
	mask_visible_npc = (mask_opaque_npc | contents_ignore_nodraw_opaque),
	mask_shot = (contents_solid | contents_moveable | contents_monster | contents_window | contents_debris | contents_hitbox),
	mask_shot_brushonly = (contents_solid | contents_moveable | contents_window | contents_debris),
	mask_shot_hull = (contents_solid | contents_moveable | contents_monster | contents_window | contents_debris | contents_grate),
	mask_shot_portal = (contents_solid | contents_moveable | contents_window | contents_monster),
	mask_solid_brushonly = (contents_solid | contents_moveable | contents_window | contents_grate),
	mask_playersolid_brushonly = (contents_solid | contents_moveable | contents_window | contents_playerclip | contents_grate),
	mask_npcsolid_brushonly = (contents_solid | contents_moveable | contents_window | contents_monsterclip | contents_grate),
	mask_npcworldstatic = (contents_solid | contents_window | contents_monsterclip | contents_grate),
	mask_npcworldstatic_fluid = (contents_solid | contents_window | contents_monsterclip),
	mask_splitareaportal = (contents_water | contents_slime),
	mask_current = (contents_current0 | contents_current90 | contents_current180 | contents_current270 | contents_current_up | contents_current_down),
	mask_deadsolid = (contents_solid | contents_playerclip | contents_window | contents_grate)
};