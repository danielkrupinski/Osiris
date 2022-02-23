#pragma once

#include "../SDK/ItemSchema.h"
#include "../SDK/WeaponId.h"
#include "StaticData.h"
#include "StaticDataStorage.h"
#include "../Helpers.h"

class StaticDataContainer {
private:
    auto findItems(WeaponId weaponID) const noexcept
    {
        struct Comp {
            bool operator()(WeaponId weaponID, const StaticData::GameItem& item) const noexcept { return weaponID < item.weaponID; }
            bool operator()(const StaticData::GameItem& item, WeaponId weaponID) const noexcept { return item.weaponID < weaponID; }
        };

        return std::equal_range(storage.getGameItems().cbegin(), storage.getGameItems().cend(), weaponID, Comp{}); // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
    }

public:
    StaticDataContainer() = default;
    explicit StaticDataContainer(StaticDataStorage dataStorage) : storage{ sorted(std::move(dataStorage)) }
    {
        const auto stickers = findItems(WeaponId::Sticker);
        tournamentStickersSorted = { stickers.first, stickers.second };

        std::ranges::sort(tournamentStickersSorted, [this](const StaticData::GameItem& itemA, const StaticData::GameItem& itemB) {
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
            return itemA.rarity > itemB.rarity;
        });
    }

    const StaticDataStorage& getStorage() const noexcept
    {
        return storage;
    }

private:
    auto getTournamentStickers(std::uint32_t tournamentID) const noexcept
    {
        // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
        const auto begin = std::lower_bound(tournamentStickersSorted.begin(), tournamentStickersSorted.end(), tournamentID, [this](const StaticData::GameItem& item, std::uint32_t tournamentID) {
            return storage.getStickerKit(item).tournamentID < tournamentID;
        });

        const auto end = std::upper_bound(tournamentStickersSorted.begin(), tournamentStickersSorted.end(), tournamentID, [this](std::uint32_t tournamentID, const StaticData::GameItem& item) {
            return storage.getStickerKit(item).tournamentID > tournamentID;
        });

        return std::make_pair(begin, end);
    }

public:
    int getTournamentEventStickerID(std::uint32_t tournamentID) const noexcept
    {
        if (tournamentID == 1) // DreamHack 2013
            return Helpers::random(1, 12);
        else if (tournamentID == 3) // EMS One Katowice 2014
            return Helpers::random(99, 100); // EMS Wolf / Skull
        else if (tournamentID == 4) // ELS One Cologne 2014
            return 172;

        const auto it = getTournamentStickers(tournamentID).first;
        if (it == tournamentStickersSorted.end())
            return 0;
        return storage.getStickerKit(*it).tournamentID == tournamentID ? storage.getStickerKit(*it).id : 0;
    }

    int getTournamentTeamGoldStickerID(std::uint32_t tournamentID, TournamentTeam team) const noexcept
    {
        if (tournamentID == 0 || team == TournamentTeam::None)
            return 0;

        if (team == TournamentTeam::AllStarTeamAmerica)
            return 1315;
        if (team == TournamentTeam::AllStarTeamEurope)
            return 1316;

        const auto range = getTournamentStickers(tournamentID);

        const auto it = std::ranges::lower_bound(range.first, range.second, team, {}, [this](const StaticData::GameItem& item) {
            return storage.getStickerKit(item).tournamentTeam;
        });
        if (it == range.second)
            return 0;
        return storage.getStickerKit(*it).tournamentTeam == team ? storage.getStickerKit(*it).id : 0;
    }

    int getTournamentPlayerGoldStickerID(std::uint32_t tournamentID, int tournamentPlayerID) const noexcept
    {
        const auto range = getTournamentStickers(tournamentID);
        const auto it = std::ranges::find(range.first, range.second, tournamentPlayerID, [this](const StaticData::GameItem& item) { return storage.getStickerKit(item).tournamentPlayerID; });
        return (it != range.second ? storage.getStickerKit(*it).id : 0);
    }

    [[nodiscard]] StaticData::ItemIndex2 getItemIndex(WeaponId weaponID, int paintKit) const noexcept
    {
        const auto [begin, end] = findItems(weaponID);
        if (const auto it = std::lower_bound(begin, end, paintKit, [this](const StaticData::GameItem& item, int paintKit) { return item.hasPaintKit() && storage.getPaintKit(item).id < paintKit; }); it != end && it->weaponID == weaponID && (!it->hasPaintKit() || storage.getPaintKit(*it).id == paintKit))
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] StaticData::ItemIndex2 getItemIndex(WeaponId weaponID) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(storage.getGameItems(), weaponID, {}, &StaticData::GameItem::weaponID); it != storage.getGameItems().end())
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const StaticData::GameItem>> getItem(WeaponId weaponID) const noexcept
    {
        if (const auto it = std::ranges::lower_bound(storage.getGameItems(), weaponID, {}, &StaticData::GameItem::weaponID); it != storage.getGameItems().end())
            return *it;
        return {};
    }

    [[nodiscard]] StaticData::ItemIndex2 getMusicIndex(int musicKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::MusicKit);
        if (const auto it = std::find_if(begin, end, [this, musicKit](const StaticData::GameItem& item) { return storage.getMusicKit(item).id == musicKit; }); it != end)
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const StaticData::GameItem>> getMusic(int musicKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::MusicKit);
        if (const auto it = std::find_if(begin, end, [this, musicKit](const StaticData::GameItem& item) { return storage.getMusicKit(item).id == musicKit; }); it != end)
            return *it;
        return {};
    }

    [[nodiscard]] StaticData::ItemIndex2 getStickerIndex(int stickerKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::Sticker);
        if (const auto it = std::find_if(begin, end, [this, stickerKit](const StaticData::GameItem& item) { return storage.getStickerKit(item).id == stickerKit; }); it != end)
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const StaticData::GameItem>> getSticker(int stickerKit) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::Sticker);
        if (const auto it = std::find_if(begin, end, [this, stickerKit](const StaticData::GameItem& item) { return storage.getStickerKit(item).id == stickerKit; }); it != end)
            return *it;
        return {};
    }

    [[nodiscard]] StaticData::ItemIndex2 getGraffitiIndex(int graffitiID) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::Graffiti);
        if (const auto it = std::find_if(begin, end, [this, graffitiID](const StaticData::GameItem& item) { return storage.getGraffitiKit(item).id == graffitiID; }); it != end)
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

    [[nodiscard]] StaticData::ItemIndex2 getSealedGraffitiIndex(int graffitiID) const noexcept
    {
        const auto [begin, end] = findItems(WeaponId::SealedGraffiti);
        if (const auto it = std::find_if(begin, end, [this, graffitiID](const StaticData::GameItem& item) { return storage.getGraffitiKit(item).id == graffitiID; }); it != end)
            return StaticData::ItemIndex2{ static_cast<std::size_t>(std::distance(storage.getGameItems().begin(), it)) };
        return StaticData::InvalidItemIdx2;
    }

private:
    [[nodiscard]] static StaticDataStorage sorted(StaticDataStorage storage)
    {
        std::ranges::sort(storage.getGameItems(), [&storage](const StaticData::GameItem& itemA, const StaticData::GameItem& itemB) {
            if (itemA.weaponID == itemB.weaponID && itemA.hasPaintKit() && itemB.hasPaintKit())
                return storage.getPaintKit(itemA).id < storage.getPaintKit(itemB).id;
            return itemA.weaponID < itemB.weaponID;
        });
        return storage;
    }

    StaticDataStorage storage;
    std::vector<std::reference_wrapper<const StaticData::GameItem>> tournamentStickersSorted;
};
