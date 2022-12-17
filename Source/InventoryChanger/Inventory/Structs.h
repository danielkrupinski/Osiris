#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <CSGO/Constants/ProPlayer.h>
#include <CSGO/Constants/Tournament.h>
#include <CSGO/Constants/TournamentTeam.h>
#include <CSGO/ItemSchema.h>

namespace csgo
{
    enum class StickerId : int;
    enum class TournamentStage : std::uint8_t;
}

namespace inventory_changer::inventory
{

struct Skin {
    struct Sticker {
        csgo::StickerId stickerID{};
        float wear = 0.0f;
    };

    float wear = 0.0f;
    int seed = 1;
    int statTrak = -1;
    csgo::Tournament tournamentID{};
    std::array<Sticker, 5> stickers;
    std::string nameTag;
    csgo::TournamentStage tournamentStage{};
    csgo::TournamentTeam tournamentTeam1{};
    csgo::TournamentTeam tournamentTeam2{};
    csgo::ProPlayer proPlayer{};

    [[nodiscard]] bool isSouvenir() const noexcept { return tournamentID != csgo::Tournament{}; }
};

using SkinStickers = decltype(Skin::stickers);

struct Agent {
    struct Patch {
        int patchID = 0;
    };

    std::array<Patch, 5> patches;
};

struct Gloves {
    float wear = 0.0f;
    int seed = 1;
};

struct Music {
    int statTrak = -1;
};

struct SouvenirPackage {
    csgo::TournamentStage tournamentStage{};
    csgo::TournamentTeam tournamentTeam1{};
    csgo::TournamentTeam tournamentTeam2{};
    csgo::ProPlayer proPlayer{};
};

struct ServiceMedal {
    std::uint32_t issueDateTimestamp = 0;
};

struct TournamentCoin {
    std::uint32_t dropsAwarded = 0;
};

struct Graffiti {
    std::int8_t usesLeft = -1;
};

struct StorageUnit {
    std::uint32_t modificationDateTimestamp = 0;
    std::uint32_t itemCount = 0;
    std::string name;
};

}
