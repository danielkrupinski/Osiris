#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include <range/v3/algorithm/equal_range.hpp>

#include "ItemSorter.h"
#include "Storage.h"
#include <Helpers.h>
#include <SDK/Constants/Tournament.h>

namespace inventory_changer::game_items
{

class Lookup {
public:
    Lookup() = default;
    explicit Lookup(Storage dataStorage) : storage{ sorted(std::move(dataStorage)) }
    {
        const auto items = std::as_const(storage).getItems();
        const auto stickerPartition = std::ranges::partition_point(items, [](const Item& item) { return item.isSticker(); });
        const auto musicPartition = std::ranges::partition_point(stickerPartition, items.end(), [](const Item& item) { return item.isMusic(); });
        const auto graffitiPartition = std::ranges::partition_point(musicPartition, items.end(), [](const Item& item) { return item.isGraffiti(); });
        const auto patchesPartition = std::ranges::partition_point(graffitiPartition, items.end(), [](const Item& item) { return item.isPatch(); });
        const auto itemsWithPaintKitPartition = std::ranges::partition_point(patchesPartition, items.end(), [this](const Item& item) { return storage.hasPaintKit(item); });

        stickers = { items.begin(), stickerPartition };
        music = { stickerPartition, musicPartition };
        graffiti = { musicPartition, graffitiPartition };
        patches = { graffitiPartition, patchesPartition };
        itemsWithPaintKit = { patchesPartition, itemsWithPaintKitPartition };
        otherItems = { itemsWithPaintKitPartition, items.end() };

        tournamentStickersSorted = { stickers.begin(), stickers.end() };

        std::ranges::sort(tournamentStickersSorted, [this](const Item& itemA, const Item& itemB) {
            assert(itemA.isSticker() && itemB.isSticker());

            const auto& paintKitA = storage.getStickerKit(itemA);
            const auto& paintKitB = storage.getStickerKit(itemB);
            if (paintKitA.tournament != paintKitB.tournament)
                return paintKitA.tournament < paintKitB.tournament;
            if (paintKitA.tournamentTeam != paintKitB.tournamentTeam)
                return paintKitA.tournamentTeam < paintKitB.tournamentTeam;
            if (paintKitA.tournamentPlayerID != paintKitB.tournamentPlayerID)
                return paintKitA.tournamentPlayerID < paintKitB.tournamentPlayerID;
            if (paintKitA.isGoldenSticker != paintKitB.isGoldenSticker)
                return paintKitA.isGoldenSticker;
            return itemA.getRarity() > itemB.getRarity();
        });
    }

    const Storage& getStorage() const noexcept
    {
        return storage;
    }

private:
    auto findTournamentStickers(csgo::Tournament tournament) const noexcept
    {
        return ranges::equal_range(tournamentStickersSorted, tournament, {}, [this](const Item& item) { return storage.getStickerKit(item).tournament; });
    }

public:
    int findTournamentEventStickerID(csgo::Tournament tournament) const noexcept
    {
        if (tournament == csgo::Tournament::DreamHack2013)
            return Helpers::random(1, 12);
        else if (tournament == csgo::Tournament::EmsOneKatowice2014)
            return Helpers::random(99, 100); // EMS Wolf / Skull
        else if (tournament == csgo::Tournament::EslOneCologne2014)
            return 172;

        const auto it = findTournamentStickers(tournament).begin();
        if (it == tournamentStickersSorted.end())
            return 0;
        return storage.getStickerKit(*it).tournament == tournament ? storage.getStickerKit(*it).id : 0;
    }

    int findTournamentTeamGoldStickerID(csgo::Tournament tournament, csgo::TournamentTeam team) const noexcept
    {
        if (tournament == csgo::Tournament{} || team == csgo::TournamentTeam::None)
            return 0;

        if (team == csgo::TournamentTeam::AllStarTeamAmerica)
            return 1315;
        if (team == csgo::TournamentTeam::AllStarTeamEurope)
            return 1316;

        const auto range = findTournamentStickers(tournament);

        const auto it = std::ranges::lower_bound(range, team, {}, [this](const Item& item) {
            return storage.getStickerKit(item).tournamentTeam;
        });
        if (it == range.end())
            return 0;
        return storage.getStickerKit(*it).tournamentTeam == team ? storage.getStickerKit(*it).id : 0;
    }

    int findTournamentPlayerGoldStickerID(csgo::Tournament tournament, int tournamentPlayerID) const noexcept
    {
        const auto range = findTournamentStickers(tournament);
        const auto it = std::ranges::find(range, tournamentPlayerID, [this](const Item& item) { return storage.getStickerKit(item).tournamentPlayerID; });
        return (it != range.end() ? storage.getStickerKit(*it).id : 0);
    }

    [[nodiscard]] const Item* findItem(WeaponId weaponID, int paintKit) const noexcept
    {
        const auto range = ranges::equal_range(itemsWithPaintKit, weaponID, {}, [](const Item& item) { return item.getWeaponID(); });
        return find(range, paintKit, [this](const Item& item) { return storage.getPaintKit(item).id; });
    }

    [[nodiscard]] const Item* findItem(WeaponId weaponID) const noexcept
    {
        return find(otherItems, weaponID, [](const Item& item) { return item.getWeaponID(); });
    }

    [[nodiscard]] const Item* findMusic(int musicKit) const noexcept
    {
        return find(music, musicKit, [this](const Item& item) { return storage.getMusicKit(item).id; });
    }

    [[nodiscard]] const Item* findSticker(int stickerKit) const noexcept
    {
        return find(stickers, stickerKit, [this](const Item& item) { return storage.getStickerKit(item).id; });
    }

    [[nodiscard]] const Item* findGraffiti(int graffitiID) const noexcept
    {
        return find(graffiti, graffitiID, [this](const Item& item) { return storage.getGraffitiKit(item).id; });
    }

    [[nodiscard]] const Item* findPatch(int patchID) const noexcept
    {
        return find(patches, patchID, [this](const Item& item) { return storage.getPatch(item).id; });
    }

private:
    template <typename Container, typename T, typename Projection>
    [[nodiscard]] static const Item* find(const Container& container, const T& value, Projection projection)
    {
        if (const auto it = std::ranges::lower_bound(container, value, {}, projection); it != std::end(container) && std::invoke(projection, *it) == value)
            return std::to_address(it);
        return nullptr;
    }

    [[nodiscard]] static Storage sorted(Storage storage)
    {
        std::ranges::sort(storage.getItems(), ItemSorter{ storage });
        return storage;
    }

    Storage storage;
    std::span<const Item> stickers;
    std::span<const Item> music;
    std::span<const Item> graffiti;
    std::span<const Item> patches;
    std::span<const Item> itemsWithPaintKit;
    std::span<const Item> otherItems;
    std::vector<std::reference_wrapper<const Item>> tournamentStickersSorted;
};

}
