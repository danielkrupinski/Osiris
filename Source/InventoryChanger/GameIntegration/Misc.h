#pragma once

#include <string_view>

#include <InventoryChanger/GameItems/Enums.h>

#include <CSGO/Constants/StickerId.h>

namespace inventory_changer::game_integration
{
    constexpr TournamentMap getTournamentMapOfSouvenirPackage(std::string_view lootListName) noexcept
    {
        if (lootListName.ends_with("de_dust2"))
            return TournamentMap::Dust2;
        if (lootListName.ends_with("de_mirage"))
            return TournamentMap::Mirage;
        if (lootListName.ends_with("de_inferno"))
            return TournamentMap::Inferno;
        if (lootListName.ends_with("de_cbble"))
            return TournamentMap::Cobblestone;
        if (lootListName.ends_with("de_overpass"))
            return TournamentMap::Overpass;
        if (lootListName.ends_with("de_cache"))
            return TournamentMap::Cache;
        if (lootListName.ends_with("de_train"))
            return TournamentMap::Train;
        if (lootListName.ends_with("de_nuke"))
            return TournamentMap::Nuke;
        if (lootListName.ends_with("de_vertigo"))
            return TournamentMap::Vertigo;
        if (lootListName.ends_with("de_ancient"))
            return TournamentMap::Ancient;
        return TournamentMap{};
    }

    [[nodiscard]] constexpr csgo::StickerId getTournamentMapGoldStickerID(TournamentMap map) noexcept
    {
        switch (map) {
        using enum TournamentMap;
        using enum csgo::StickerId;
        case Ancient: return AncientGold;
        case Dust2: return Dust2Gold;
        case Inferno: return InfernoGold;
        case Mirage: return MirageGold;
        case Nuke: return NukeGold;
        case Overpass: return OverpassGold;
        case Vertigo: return VertigoGold;
        default: return Default;
        }
    }

}
