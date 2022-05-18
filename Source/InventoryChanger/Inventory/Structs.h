#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <SDK/ItemSchema.h>

namespace inventory
{

struct Skin {
    struct Sticker {
        int stickerID = 0;
        float wear = 0.0f;
    };

    float wear = 0.0f;
    int seed = 1;
    int statTrak = -1;
    std::uint32_t tournamentID = 0;
    std::array<Sticker, 5> stickers;
    std::string nameTag;
    TournamentStage tournamentStage{};
    TournamentTeam tournamentTeam1{};
    TournamentTeam tournamentTeam2{};
    ProPlayer proPlayer{};

    [[nodiscard]] bool isSouvenir() const noexcept { return tournamentID != 0; }
};

struct Agent {
    struct Patch {
        int patchID = 0;
    };

    std::array<Patch, 5> patches;
};

struct Glove {
    float wear = 0.0f;
    int seed = 1;
};

struct Music {
    int statTrak = -1;
};

struct SouvenirPackage {
    TournamentStage tournamentStage{};
    TournamentTeam tournamentTeam1{};
    TournamentTeam tournamentTeam2{};
    ProPlayer proPlayer{};
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

}
