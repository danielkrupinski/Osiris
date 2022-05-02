#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include <range/v3/algorithm/equal_range.hpp>

#include "ItemSorter.h"
#include "Storage.h"

namespace game_items
{

class Lookup {
private:
    auto findItems(WeaponId weaponID) const noexcept
    {
        return ranges::equal_range(storage.getItems(), weaponID, {}, &Item::getWeaponID);
    }

public:
    Lookup() = default;
    explicit Lookup(Storage dataStorage) : storage{ sorted(std::move(dataStorage)) }
    {
        itemsWithPaintKit = { storage.getItems().begin(),
        std::ranges::partition_point(storage.getItems(), [this](const Item& item) { return storage.hasPaintKit(item); }) };

        for (const auto& item : storage.getItems()) {
            if (item.isSticker())
                stickersSorted.emplace_back(item);
            else if (item.isGraffiti())
                graffitiSorted.emplace_back(item);
            else if (item.isMusic())
                musicKitsSorted.emplace_back(item);
            else if (item.isPatch())
                patchesSorted.emplace_back(item);
        }

        std::ranges::sort(stickersSorted, {}, [this](const Item& item) { return storage.getStickerKit(item).id; });
        stickersSorted.shrink_to_fit();

        std::ranges::sort(musicKitsSorted, {}, [this](const Item& item) { return storage.getMusicKit(item).id; });
        musicKitsSorted.shrink_to_fit();

        std::ranges::sort(graffitiSorted, {}, [this](const Item& item) { return storage.getGraffitiKit(item).id; });
        graffitiSorted.shrink_to_fit();

        std::ranges::sort(patchesSorted, {}, [this](const Item& item) { return storage.getPatch(item).id; });
        patchesSorted.shrink_to_fit();

        tournamentStickersSorted = stickersSorted;

        std::ranges::sort(tournamentStickersSorted, [this](const Item& itemA, const Item& itemB) {
            assert(itemA.isSticker() && itemB.isSticker());

            const auto& paintKitA = storage.getStickerKit(itemA);
            const auto& paintKitB = storage.getStickerKit(itemB);
            if (paintKitA.tournamentID != paintKitB.tournamentID)
                return paintKitA.tournamentID < paintKitB.tournamentID;
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
    auto findTournamentStickers(std::uint32_t tournamentID) const noexcept
    {
        return ranges::equal_range(tournamentStickersSorted, tournamentID, {}, [this](const Item& item) { return storage.getStickerKit(item).tournamentID; });
    }

public:
    int findTournamentEventStickerID(std::uint32_t tournamentID) const noexcept
    {
        if (tournamentID == 1) // DreamHack 2013
            return Helpers::random(1, 12);
        else if (tournamentID == 3) // EMS One Katowice 2014
            return Helpers::random(99, 100); // EMS Wolf / Skull
        else if (tournamentID == 4) // ELS One Cologne 2014
            return 172;

        const auto it = findTournamentStickers(tournamentID).begin();
        if (it == tournamentStickersSorted.end())
            return 0;
        return storage.getStickerKit(*it).tournamentID == tournamentID ? storage.getStickerKit(*it).id : 0;
    }

    int findTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) const noexcept
    {
        if (tournamentID == 0 || team == TournamentTeam::None)
            return 0;

        if (team == TournamentTeam::AllStarTeamAmerica)
            return 1315;
        if (team == TournamentTeam::AllStarTeamEurope)
            return 1316;

        const auto range = findTournamentStickers(tournamentID);

        const auto it = std::ranges::lower_bound(range, team, {}, [this](const Item& item) {
            return storage.getStickerKit(item).tournamentTeam;
        });
        if (it == range.end())
            return 0;
        return storage.getStickerKit(*it).tournamentTeam == team ? storage.getStickerKit(*it).id : 0;
    }

    int findTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) const noexcept
    {
        const auto range = findTournamentStickers(tournamentID);
        const auto it = std::ranges::find(range, tournamentPlayerID, [this](const Item& item) { return storage.getStickerKit(item).tournamentPlayerID; });
        return (it != range.end() ? storage.getStickerKit(*it).id : 0);
    }

    using ItemReference = std::reference_wrapper<const Item>;
    using OptionalItemReference = std::optional<ItemReference>;

    [[nodiscard]] OptionalItemReference findItem(WeaponId weaponID, int paintKit) const noexcept
    {
        const auto range = ranges::equal_range(itemsWithPaintKit, weaponID, {}, &Item::getWeaponID);

        if (const auto it = std::ranges::lower_bound(range, paintKit, {}, [this](const Item& item) { return storage.getPaintKit(item).id; }); it != range.end() && storage.getPaintKit(*it).id == paintKit)
            return *it;
        return {};
    }

    [[nodiscard]] OptionalItemReference findItem(WeaponId weaponID) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(storage.getItems(), weaponID, {}, &Item::getWeaponID); it != storage.getItems().end() && it->getWeaponID() == weaponID)
            return *it;
        return {};
    }

    [[nodiscard]] OptionalItemReference findMusic(int musicKit) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(musicKitsSorted, musicKit, {}, [this](const Item& item) { return storage.getMusicKit(item).id; }); it != musicKitsSorted.end() && storage.getMusicKit(*it).id == musicKit)
            return *it;
        return {};
    }

    [[nodiscard]] OptionalItemReference findSticker(int stickerKit) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(stickersSorted, stickerKit, {}, [this](const Item& item) { return storage.getStickerKit(item).id; }); it != stickersSorted.end() && storage.getStickerKit(*it).id == stickerKit)
            return *it;
        return {};
    }

    [[nodiscard]] OptionalItemReference findGraffiti(int graffitiID) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(graffitiSorted, graffitiID, {}, [this](const Item& item) { return storage.getGraffitiKit(item).id; }); it != graffitiSorted.end() && storage.getGraffitiKit(*it).id == graffitiID)
            return *it;
        return {};
    }

    [[nodiscard]] OptionalItemReference findPatch(int patchID) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(patchesSorted, patchID, {}, [this](const Item& item) { return storage.getPatch(item).id; }); it != patchesSorted.end() && storage.getPatch(*it).id == patchID)
            return *it;
        return {};
    }

private:
    [[nodiscard]] static Storage sorted(Storage storage)
    {
        std::ranges::sort(storage.getItems(), ItemSorter{ storage });
        return storage;
    }

    Storage storage;
    std::span<const Item> itemsWithPaintKit;
    std::vector<ItemReference> stickersSorted;
    std::vector<ItemReference> tournamentStickersSorted;
    std::vector<ItemReference> musicKitsSorted;
    std::vector<ItemReference> graffitiSorted;
    std::vector<ItemReference> patchesSorted;
};

}
