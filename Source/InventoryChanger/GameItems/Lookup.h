#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include "Storage.h"

namespace game_items
{

class Lookup {
private:
    auto findItems(WeaponId weaponID) const noexcept
    {
        struct Comp {
            bool operator()(WeaponId weaponID, const Item& item) const noexcept { return weaponID < item.getWeaponID(); }
            bool operator()(const Item& item, WeaponId weaponID) const noexcept { return item.getWeaponID() < weaponID; }
        };

        return std::equal_range(storage.getItems().begin(), storage.getItems().end(), weaponID, Comp{}); // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
    }

public:
    Lookup() = default;
    explicit Lookup(Storage dataStorage) : storage{ sorted(std::move(dataStorage)) }
    {
        const auto stickers = findItems(WeaponId::Sticker);
        tournamentStickersSorted = { stickers.first, stickers.second };

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
        // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
        const auto begin = std::ranges::lower_bound(tournamentStickersSorted, tournamentID, {}, [this](const Item& item) { return storage.getStickerKit(item).tournamentID; });
        const auto end = std::ranges::upper_bound(tournamentStickersSorted, tournamentID, {}, [this](const Item& item) { return storage.getStickerKit(item).tournamentID; });

        return std::make_pair(begin, end);
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

        const auto it = findTournamentStickers(tournamentID).first;
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

        const auto it = std::ranges::lower_bound(range.first, range.second, team, {}, [this](const Item& item) {
            return storage.getStickerKit(item).tournamentTeam;
        });
        if (it == range.second)
            return 0;
        return storage.getStickerKit(*it).tournamentTeam == team ? storage.getStickerKit(*it).id : 0;
    }

    int findTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) const noexcept
    {
        const auto range = findTournamentStickers(tournamentID);
        const auto it = std::ranges::find(range.first, range.second, tournamentPlayerID, [this](const Item& item) { return storage.getStickerKit(item).tournamentPlayerID; });
        return (it != range.second ? storage.getStickerKit(*it).id : 0);
    }

    using ItemReference = std::reference_wrapper<const Item>;
    using OptionalItemReference = std::optional<ItemReference>;

    [[nodiscard]] OptionalItemReference findItem(WeaponId weaponID, int paintKit) const noexcept
    {
        const auto [begin, end] = findItems(weaponID);
        if (begin != end && !begin->isSkin() && !begin->isGloves())
            return {};

        if (const auto it = std::ranges::lower_bound(begin, end, paintKit, {}, [this](const Item& item) { return storage.getPaintKit(item).id; }); it != end && storage.getPaintKit(*it).id == paintKit)
            return *it;
        return {};
    }

    [[nodiscard]] OptionalItemReference findItem(WeaponId weaponID) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(storage.getItems(), weaponID, {}, &Item::getWeaponID); it != storage.getItems().end())
            return *it;
        return {};
    }

    [[nodiscard]] OptionalItemReference findMusic(int musicKit) const noexcept
    {
        return findItem(WeaponId::MusicKit, musicKit, [this](const Item& item) { return storage.getMusicKit(item).id; });
    }

    [[nodiscard]] OptionalItemReference findSticker(int stickerKit) const noexcept
    {
        return findItem(WeaponId::Sticker, stickerKit, [this](const Item& item) { return storage.getStickerKit(item).id; });
    }

    [[nodiscard]] OptionalItemReference findGraffiti(int graffitiID) const noexcept
    {
        return findItem(WeaponId::SealedGraffiti, graffitiID, [this](const Item& item) { return storage.getGraffitiKit(item).id; });
    }

    [[nodiscard]] OptionalItemReference findPatch(int patchID) const noexcept
    {
        return findItem(WeaponId::Patch, patchID, [this](const Item& item) { return storage.getPatchKit(item).id; });
    }

private:
    template <typename T, typename Projection>
    [[nodiscard]] OptionalItemReference findItem(WeaponId weaponID, const T& value, Projection projection) const
    {
        const auto [begin, end] = findItems(weaponID);
        if (const auto it = std::ranges::find(begin, end, value, projection); it != end)
            return *it;
        return {};
    }

    [[nodiscard]] static Storage sorted(Storage storage)
    {
        std::ranges::sort(storage.getItems(), [&storage](const Item& itemA, const Item& itemB) {
            if (itemA.getWeaponID() == itemB.getWeaponID() && (itemA.isSkin() || itemA.isGloves()) && (itemB.isSkin() || itemB.isGloves()))
                return storage.getPaintKit(itemA).id < storage.getPaintKit(itemB).id;
            return itemA.getWeaponID() < itemB.getWeaponID();
        });
        return storage;
    }

    Storage storage;
    std::vector<ItemReference> tournamentStickersSorted;
};

}
