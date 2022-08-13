#pragma once

#include <string_view>

#include <SDK/Constants/Tournament.h>
#include <SDK/Constants/TournamentTeam.h>

namespace inventory_changer::game_items
{

struct ItemName {
    std::string_view forDisplay;
    std::wstring_view forSearch;
};

struct MusicKit {
    MusicKit(int id, ItemName name) : id{ id }, name{ name } {}

    int id;
    ItemName name;
};

struct GraffitiKit {
    GraffitiKit(int id, ItemName name) : id{ id }, name{ name } {}

    int id;
    ItemName name;
};

struct Patch {
    Patch(int id, ItemName name) : id{ id }, name{ name } {}

    int id;
    ItemName name;
};

struct StickerKit {
    StickerKit(int id, ItemName name, csgo::Tournament tournament, csgo::TournamentTeam tournamentTeam, int tournamentPlayerID, bool isGoldenSticker)
        : id{ id }, name{ name }, tournament{ tournament }, tournamentTeam{ tournamentTeam }, isGoldenSticker{ isGoldenSticker }, tournamentPlayerID{ tournamentPlayerID } {}

    int id;
    ItemName name;
    csgo::Tournament tournament{};
    csgo::TournamentTeam tournamentTeam{};
    bool isGoldenSticker = false;
    int tournamentPlayerID = 0;
};

struct PaintKit {
    PaintKit(int id, ItemName name, float wearRemapMin, float wearRemapMax) noexcept
        : id{ id }, wearRemapMin{ wearRemapMin }, wearRemapMax{ wearRemapMax }, name{ name } {}

    int id;
    float wearRemapMin = 0.0f;
    float wearRemapMax = 1.0f;
    ItemName name;
};

}
