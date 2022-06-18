#pragma once

#include <string_view>

#include <InventoryChanger/GameItems/Enums.h>

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
        return TournamentMap::None;
    }

    constexpr int getTournamentMapGoldStickerID(TournamentMap map) noexcept
    {
        switch (map) {
        case TournamentMap::Ancient: return 1689;
        case TournamentMap::Dust2: return 1690;
        case TournamentMap::Inferno: return 1691;
        case TournamentMap::Mirage: return 1692;
        case TournamentMap::Nuke: return 1693;
        case TournamentMap::Overpass: return 1694;
        case TournamentMap::Vertigo: return 1695;
        default: return 0;
        }
    }

}
