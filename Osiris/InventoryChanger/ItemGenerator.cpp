#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <random>
#include <span>
#include <vector>

#include "../Helpers.h"
#include "Inventory.h"
#include "ItemGenerator.h"

#include "../SDK/ItemSchema.h"
#include "TournamentMatches.h"
#include "GameItems/Lookup.h"

static float generateWear() noexcept
{
    float wear;
    if (const auto condition = Helpers::random(1, 10000); condition <= 1471)
        wear = Helpers::random(0.0f, 0.07f);
    else if (condition <= 3939)
        wear = Helpers::random(0.07f, 0.15f);
    else if (condition <= 8257)
        wear = Helpers::random(0.15f, 0.38f);
    else if (condition <= 9049)
        wear = Helpers::random(0.38f, 0.45f);
    else
        wear = Helpers::random(0.45f, 1.0f);
    return wear;
}

using StaticData::TournamentMap;

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(WeaponId weaponID, std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2, ProPlayer player) noexcept;

template <typename Integral, std::size_t N>
[[nodiscard]] constexpr auto normalizedFloatsToIntegers(const std::array<float, N>& floats) noexcept
{
    std::array<Integral, N> integers;
    for (std::size_t i = 0; i < N; ++i)
        integers[i] = static_cast<Integral>(floats[i] * (std::numeric_limits<Integral>::max)());
    return integers;
}

class DropRate {
public:
    static constexpr auto MaxDistinctRarities = 6; // in Cobblestone souvenir package, gray to red
    using T = std::uint16_t;

    constexpr DropRate(StaticData::EconRarities rarities, const std::array<float, MaxDistinctRarities>& chances)
        : rarities{ rarities }, chances{ normalizedFloatsToIntegers<T>(chances) } {}

    [[nodiscard]] constexpr EconRarity mapToRarity(T number) const
    {
        for (std::uint8_t i = 0; i < chances.size(); ++i) {
            if (number < chances[i])
                return rarities.getNthRarity(i);
            number -= chances[i];
        }
        return rarities.getNthRarity(0);
    }

    StaticData::EconRarities rarities;
    std::array<T, MaxDistinctRarities> chances{};
};

constexpr auto dropRates = std::to_array<DropRate>({
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Pink, EconRarity::Red }, { 0.7992f, 0.1598f, 0.032f, 0.0064f } },
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Pink, EconRarity::Red, EconRarity::Gold }, { 0.7992f, 0.1598f, 0.032f, 0.0064f, 0.0026f } },
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue }, { 0.7992f, 0.1598f, 0.041f } }, // Inferno souvenir package
    { { EconRarity::Blue, EconRarity::Purple }, { 0.80f, 0.20f } }, // Stockholm 2021 Team Patch Packs, Sticker capsules: Pinups, Sugarface, Poorly Drawn, Recoil
    { { EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple }, { 0.7992f, 0.1598f, 0.041f } }, // Cache souvenir package
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple }, { 0.80f, 0.16f, 0.03f, 0.01f } }, // Souvenir package: Nuke, Mirage, Train, Vertigo
    { { EconRarity::Blue, EconRarity::Pink }, { 0.95f, 0.05f } }, // Team Roles sticker capsule
    { { EconRarity::Purple, EconRarity::Pink }, { 0.80f, 0.20f } }, // Tournament Team (holo / foil) sticker capsules
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Pink }, { 0.7992f, 0.1598f, 0.041f } }, // Regular sticker capsules
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple, EconRarity::Pink }, { 0.80f, 0.16f, 0.032f, 0.0064f, 0.0016f } }, // DreamHack2013 Collections, Souvenir package: Nuke 2018, Inferno 2018, Overpass, Dust 2
    { { EconRarity::Blue, EconRarity::Purple, EconRarity::Red }, { 0.8013f, 0.1923f, 0.0064f } }, // Autograph sticker capsules
    { { EconRarity::Gray, EconRarity::LightBlue, EconRarity::Blue, EconRarity::Purple, EconRarity::Pink, EconRarity::Red }, { 0.80f, 0.16f, 0.032f, 0.0064f, 0.0012f, 0.0004f } }, // Souvenir package: Cobblestone, Ancient, Vertigo 2021, Mirage 2021, Dust 2 2021
});

struct RareSpecialItem {
    WeaponId weaponID;
    int paintKit;
};

constexpr auto operationRiptideKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Bowie, 1104 }, // Lore
    { WeaponId::Bowie, 568 }, // Gamma Doppler Emerald
    { WeaponId::Bowie, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Bowie, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Bowie, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Bowie, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Bowie, 1114 }, // Autotronic
    { WeaponId::Bowie, 1109 }, // Black Laminate
    { WeaponId::Bowie, 581 }, // Freehand
    { WeaponId::Bowie, 579 }, // Bright Water

    { WeaponId::Butterfly, 1115 }, // Autotronic
    { WeaponId::Butterfly, 579 }, // Bright Water
    { WeaponId::Butterfly, 1105 }, // Lore
    { WeaponId::Butterfly, 1110 }, // Black Laminate
    { WeaponId::Butterfly, 581 }, // Freehand
    { WeaponId::Butterfly, 568 }, // Gamma Doppler Emerald
    { WeaponId::Butterfly, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Butterfly, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Butterfly, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Butterfly, 572 }, // Gamma Doppler Phase 4

    { WeaponId::Falchion, 568 }, // Gamma Doppler Emerald
    { WeaponId::Falchion, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Falchion, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Falchion, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Falchion, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Falchion, 1106 }, // Lore
    { WeaponId::Falchion, 1116 }, // Autotronic
    { WeaponId::Falchion, 1111 }, // Black Laminate
    { WeaponId::Falchion, 579 }, // Bright Water
    { WeaponId::Falchion, 581 }, // Freehand

    { WeaponId::Huntsman, 1107 }, // Lore
    { WeaponId::Huntsman, 568 }, // Gamma Doppler Emerald
    { WeaponId::Huntsman, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Huntsman, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Huntsman, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Huntsman, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Huntsman, 1112 }, // Black Laminate
    { WeaponId::Huntsman, 1117 }, // Autotronic
    { WeaponId::Huntsman, 581 }, // Freehand
    { WeaponId::Huntsman, 579 }, // Bright Water

    { WeaponId::Daggers, 568 }, // Gamma Doppler Emerald
    { WeaponId::Daggers, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Daggers, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Daggers, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Daggers, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Daggers, 1108 }, // Lore
    { WeaponId::Daggers, 1118 }, // Autotronic
    { WeaponId::Daggers, 1113 }, // Black Laminate
    { WeaponId::Daggers, 581 }, // Freehand
    { WeaponId::Daggers, 579 }, // Bright Water
});

constexpr auto operationBrokenFangGloves = std::to_array<RareSpecialItem>({
    { WeaponId::GloveStuddedBrokenfang, 10085 }, // Jade
    { WeaponId::GloveStuddedBrokenfang, 10088 }, // Unhinged
    { WeaponId::GloveStuddedBrokenfang, 10087 }, // Needle Point
    { WeaponId::GloveStuddedBrokenfang, 10086 }, // Yellow-banded

    { WeaponId::GloveSlick, 10070 }, // Snow Leopard
    { WeaponId::GloveSlick, 10072 }, // Black Tie
    { WeaponId::GloveSlick, 10071 }, // Queen Jaguar
    { WeaponId::GloveSlick, 10069 }, // Rezan the Red

    { WeaponId::GloveLeatherWrap, 10084 }, // CAUTION!
    { WeaponId::GloveLeatherWrap, 10083 }, // Constrictor
    { WeaponId::GloveLeatherWrap, 10082 }, // Giraffe
    { WeaponId::GloveLeatherWrap, 10081 }, // Desert Shamagh

    { WeaponId::GloveMotorcycle, 10078 }, // Smoke Out
    { WeaponId::GloveMotorcycle, 10080 }, // 3rd Commando Company
    { WeaponId::GloveMotorcycle, 10077 }, // Finish Line
    { WeaponId::GloveMotorcycle, 10079 }, // Blood Pressure

    { WeaponId::GloveSpecialist, 10065 }, // Marble Fade
    { WeaponId::GloveSpecialist, 10068 }, // Field Agent
    { WeaponId::GloveSpecialist, 10067 }, // Tiger Strike
    { WeaponId::GloveSpecialist, 10066 }, // Lt. Commander

    { WeaponId::GloveSporty, 10073 }, // Slingshot
    { WeaponId::GloveSporty, 10074 }, // Big Game
    { WeaponId::GloveSporty, 10076 }, // Nocts
    { WeaponId::GloveSporty, 10075 }, // Scarlet Shamagh
});

constexpr auto shatteredWebKnives = std::to_array<RareSpecialItem>({
    { WeaponId::NomadKnife, 38 }, // Fade
    { WeaponId::NomadKnife, 59 }, // Slaughter
    { WeaponId::NomadKnife, 12 }, // Crimson Web
    { WeaponId::NomadKnife, 44 }, // Case Hardened
    { WeaponId::NomadKnife, 0 }, // (Vanilla)
    { WeaponId::NomadKnife, 42 }, // Blue Steel
    { WeaponId::NomadKnife, 143 }, // Urban Masked
    { WeaponId::NomadKnife, 43 }, // Stained
    { WeaponId::NomadKnife, 735 }, // Night Stripe
    { WeaponId::NomadKnife, 5 }, // Forest DDPAT
    { WeaponId::NomadKnife, 77 }, // Boreal Forest
    { WeaponId::NomadKnife, 175 }, // Scorched
    { WeaponId::NomadKnife, 72 }, // Safari Mesh

    { WeaponId::Paracord, 38 }, // Fade
    { WeaponId::Paracord, 59 }, // Slaughter
    { WeaponId::Paracord, 12 }, // Crimson Web
    { WeaponId::Paracord, 44 }, // Case Hardened
    { WeaponId::Paracord, 0 }, // (Vanilla)
    { WeaponId::Paracord, 42 }, // Blue Steel
    { WeaponId::Paracord, 143 }, // Urban Masked
    { WeaponId::Paracord, 43 }, // Stained
    { WeaponId::Paracord, 735 }, // Night Stripe
    { WeaponId::Paracord, 5 }, // Forest DDPAT
    { WeaponId::Paracord, 77 }, // Boreal Forest
    { WeaponId::Paracord, 175 }, // Scorched
    { WeaponId::Paracord, 72 }, // Safari Mesh

    { WeaponId::SkeletonKnife, 38 }, // Fade
    { WeaponId::SkeletonKnife, 59 }, // Slaughter
    { WeaponId::SkeletonKnife, 12 }, // Crimson Web
    { WeaponId::SkeletonKnife, 44 }, // Case Hardened
    { WeaponId::SkeletonKnife, 0 }, // (Vanilla)
    { WeaponId::SkeletonKnife, 42 }, // Blue Steel
    { WeaponId::SkeletonKnife, 143 }, // Urban Masked
    { WeaponId::SkeletonKnife, 43 }, // Stained
    { WeaponId::SkeletonKnife, 735 }, // Night Stripe
    { WeaponId::SkeletonKnife, 5 }, // Forest DDPAT
    { WeaponId::SkeletonKnife, 77 }, // Boreal Forest
    { WeaponId::SkeletonKnife, 175 }, // Scorched
    { WeaponId::SkeletonKnife, 72 }, // Safari Mesh

    { WeaponId::SurvivalKnife, 38 }, // Fade
    { WeaponId::SurvivalKnife, 59 }, // Slaughter
    { WeaponId::SurvivalKnife, 12 }, // Crimson Web
    { WeaponId::SurvivalKnife, 44 }, // Case Hardened
    { WeaponId::SurvivalKnife, 0 }, // (Vanilla)
    { WeaponId::SurvivalKnife, 42 }, // Blue Steel
    { WeaponId::SurvivalKnife, 143 }, // Urban Masked
    { WeaponId::SurvivalKnife, 43 }, // Stained
    { WeaponId::SurvivalKnife, 735 }, // Night Stripe
    { WeaponId::SurvivalKnife, 5 }, // Forest DDPAT
    { WeaponId::SurvivalKnife, 77 }, // Boreal Forest
    { WeaponId::SurvivalKnife, 175 }, // Scorched
    { WeaponId::SurvivalKnife, 72 }, // Safari Mesh
});

constexpr auto prismaKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Navaja, 415 }, // Doppler Ruby
    { WeaponId::Navaja, 416 }, // Doppler Sapphire
    { WeaponId::Navaja, 417 }, // Doppler Black Pearl
    { WeaponId::Navaja, 418 }, // Doppler Phase 1
    { WeaponId::Navaja, 419 }, // Doppler Phase 2
    { WeaponId::Navaja, 420 }, // Doppler Phase 3
    { WeaponId::Navaja, 421 }, // Doppler Phase 4
    { WeaponId::Navaja, 413 }, // Marble Fade
    { WeaponId::Navaja, 409 }, // Tiger Tooth
    { WeaponId::Navaja, 98 }, // Ultraviolet
    { WeaponId::Navaja, 857 }, // Damascus Steel
    { WeaponId::Navaja, 414 }, // Rust Coat

    { WeaponId::Stiletto, 415 }, // Doppler Ruby
    { WeaponId::Stiletto, 416 }, // Doppler Sapphire
    { WeaponId::Stiletto, 417 }, // Doppler Black Pearl
    { WeaponId::Stiletto, 418 }, // Doppler Phase 1
    { WeaponId::Stiletto, 419 }, // Doppler Phase 2
    { WeaponId::Stiletto, 420 }, // Doppler Phase 3
    { WeaponId::Stiletto, 421 }, // Doppler Phase 4
    { WeaponId::Stiletto, 413 }, // Marble Fade
    { WeaponId::Stiletto, 409 }, // Tiger Tooth
    { WeaponId::Stiletto, 98 }, // Ultraviolet
    { WeaponId::Stiletto, 857 }, // Damascus Steel
    { WeaponId::Stiletto, 414 }, // Rust Coat

    { WeaponId::Talon, 415 }, // Doppler Ruby
    { WeaponId::Talon, 416 }, // Doppler Sapphire
    { WeaponId::Talon, 417 }, // Doppler Black Pearl
    { WeaponId::Talon, 852 }, // Doppler Phase 1
    { WeaponId::Talon, 853 }, // Doppler Phase 2
    { WeaponId::Talon, 854 }, // Doppler Phase 3
    { WeaponId::Talon, 855 }, // Doppler Phase 4
    { WeaponId::Talon, 856 }, // Marble Fade
    { WeaponId::Talon, 409 }, // Tiger Tooth
    { WeaponId::Talon, 98 }, // Ultraviolet
    { WeaponId::Talon, 858 }, // Damascus Steel
    { WeaponId::Talon, 414 }, // Rust Coat

    { WeaponId::Ursus, 415 }, // Doppler Ruby
    { WeaponId::Ursus, 416 }, // Doppler Sapphire
    { WeaponId::Ursus, 417 }, // Doppler Black Pearl
    { WeaponId::Ursus, 418 }, // Doppler Phase 1
    { WeaponId::Ursus, 419 }, // Doppler Phase 2
    { WeaponId::Ursus, 420 }, // Doppler Phase 3
    { WeaponId::Ursus, 421 }, // Doppler Phase 4
    { WeaponId::Ursus, 413 }, // Marble Fade
    { WeaponId::Ursus, 409 }, // Tiger Tooth
    { WeaponId::Ursus, 98 }, // Ultraviolet
    { WeaponId::Ursus, 857 }, // Damascus Steel
    { WeaponId::Ursus, 414 }, // Rust Coat
});

constexpr auto cs20Knives = std::to_array<RareSpecialItem>({
    { WeaponId::ClassicKnife, 38 }, // Fade
    { WeaponId::ClassicKnife, 12 }, // Crimson Web
    { WeaponId::ClassicKnife, 0 }, // (Vanilla)
    { WeaponId::ClassicKnife, 44 }, // Case Hardened
    { WeaponId::ClassicKnife, 59 }, // Slaughter
    { WeaponId::ClassicKnife, 175 }, // Scorched
    { WeaponId::ClassicKnife, 735 }, // Night Stripe
    { WeaponId::ClassicKnife, 42 }, // Blue Steel
    { WeaponId::ClassicKnife, 77 }, // Boreal Forest
    { WeaponId::ClassicKnife, 43 }, // Stained
    { WeaponId::ClassicKnife, 143 }, // Urban Masked
    { WeaponId::ClassicKnife, 72 }, // Safari Mesh
    { WeaponId::ClassicKnife, 5 }, // Forest DDPAT
});

constexpr auto chromaKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Bayonet, 413 }, // Marble Fade
    { WeaponId::Bayonet, 409 }, // Tiger Tooth
    { WeaponId::Bayonet, 415 }, // Doppler Ruby
    { WeaponId::Bayonet, 416 }, // Doppler Sapphire
    { WeaponId::Bayonet, 417 }, // Doppler Black Pearl
    { WeaponId::Bayonet, 418 }, // Doppler Phase 1
    { WeaponId::Bayonet, 419 }, // Doppler Phase 2
    { WeaponId::Bayonet, 420 }, // Doppler Phase 3
    { WeaponId::Bayonet, 421 }, // Doppler Phase 4
    { WeaponId::Bayonet, 98 }, // Ultraviolet
    { WeaponId::Bayonet, 410 }, // Damascus Steel
    { WeaponId::Bayonet, 414 }, // Rust Coat

    { WeaponId::Flip, 413 }, // Marble Fade
    { WeaponId::Flip, 409 }, // Tiger Tooth
    { WeaponId::Flip, 415 }, // Doppler Ruby
    { WeaponId::Flip, 416 }, // Doppler Sapphire
    { WeaponId::Flip, 417 }, // Doppler Black Pearl
    { WeaponId::Flip, 418 }, // Doppler Phase 1
    { WeaponId::Flip, 419 }, // Doppler Phase 2
    { WeaponId::Flip, 420 }, // Doppler Phase 3
    { WeaponId::Flip, 421 }, // Doppler Phase 4
    { WeaponId::Flip, 98 }, // Ultraviolet
    { WeaponId::Flip, 410 }, // Damascus Steel
    { WeaponId::Flip, 414 }, // Rust Coat

    { WeaponId::Gut, 413 }, // Marble Fade
    { WeaponId::Gut, 409 }, // Tiger Tooth
    { WeaponId::Gut, 415 }, // Doppler Ruby
    { WeaponId::Gut, 416 }, // Doppler Sapphire
    { WeaponId::Gut, 417 }, // Doppler Black Pearl
    { WeaponId::Gut, 418 }, // Doppler Phase 1
    { WeaponId::Gut, 419 }, // Doppler Phase 2
    { WeaponId::Gut, 420 }, // Doppler Phase 3
    { WeaponId::Gut, 421 }, // Doppler Phase 4
    { WeaponId::Gut, 98 }, // Ultraviolet
    { WeaponId::Gut, 410 }, // Damascus Steel
    { WeaponId::Gut, 414 }, // Rust Coat

    { WeaponId::Karambit, 413 }, // Marble Fade
    { WeaponId::Karambit, 409 }, // Tiger Tooth
    { WeaponId::Karambit, 415 }, // Doppler Ruby
    { WeaponId::Karambit, 416 }, // Doppler Sapphire
    { WeaponId::Karambit, 417 }, // Doppler Black Pearl
    { WeaponId::Karambit, 418 }, // Doppler Phase 1
    { WeaponId::Karambit, 419 }, // Doppler Phase 2
    { WeaponId::Karambit, 420 }, // Doppler Phase 3
    { WeaponId::Karambit, 421 }, // Doppler Phase 4
    { WeaponId::Karambit, 98 }, // Ultraviolet
    { WeaponId::Karambit, 410 }, // Damascus Steel
    { WeaponId::Karambit, 414 }, // Rust Coat

    { WeaponId::M9Bayonet, 413 }, // Marble Fade
    { WeaponId::M9Bayonet, 409 }, // Tiger Tooth
    { WeaponId::M9Bayonet, 415 }, // Doppler Ruby
    { WeaponId::M9Bayonet, 416 }, // Doppler Sapphire
    { WeaponId::M9Bayonet, 417 }, // Doppler Black Pearl
    { WeaponId::M9Bayonet, 418 }, // Doppler Phase 1
    { WeaponId::M9Bayonet, 419 }, // Doppler Phase 2
    { WeaponId::M9Bayonet, 420 }, // Doppler Phase 3
    { WeaponId::M9Bayonet, 421 }, // Doppler Phase 4
    { WeaponId::M9Bayonet, 98 }, // Ultraviolet
    { WeaponId::M9Bayonet, 411 }, // Damascus Steel
    { WeaponId::M9Bayonet, 414 }, // Rust Coat
});

constexpr auto horizonKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Navaja, 38 }, // Fade
    { WeaponId::Navaja, 143 }, // Urban Masked
    { WeaponId::Navaja, 72 }, // Safari Mesh
    { WeaponId::Navaja, 12 }, // Crimson Web
    { WeaponId::Navaja, 735 }, // Night Stripe
    { WeaponId::Navaja, 44 }, // Case Hardened
    { WeaponId::Navaja, 59 }, // Slaughter
    { WeaponId::Navaja, 42 }, // Blue Steel
    { WeaponId::Navaja, 5 }, // Forest DDPAT
    { WeaponId::Navaja, 43 }, // Stained
    { WeaponId::Navaja, 175 }, // Scorched
    { WeaponId::Navaja, 77 }, // Boreal Forest
    { WeaponId::Navaja, 0 }, // (Vanilla)

    { WeaponId::Stiletto, 38 }, // Fade
    { WeaponId::Stiletto, 143 }, // Urban Masked
    { WeaponId::Stiletto, 72 }, // Safari Mesh
    { WeaponId::Stiletto, 12 }, // Crimson Web
    { WeaponId::Stiletto, 735 }, // Night Stripe
    { WeaponId::Stiletto, 44 }, // Case Hardened
    { WeaponId::Stiletto, 59 }, // Slaughter
    { WeaponId::Stiletto, 42 }, // Blue Steel
    { WeaponId::Stiletto, 5 }, // Forest DDPAT
    { WeaponId::Stiletto, 43 }, // Stained
    { WeaponId::Stiletto, 175 }, // Scorched
    { WeaponId::Stiletto, 77 }, // Boreal Forest
    { WeaponId::Stiletto, 0 }, // (Vanilla)

    { WeaponId::Talon, 38 }, // Fade
    { WeaponId::Talon, 143 }, // Urban Masked
    { WeaponId::Talon, 72 }, // Safari Mesh
    { WeaponId::Talon, 12 }, // Crimson Web
    { WeaponId::Talon, 735 }, // Night Stripe
    { WeaponId::Talon, 44 }, // Case Hardened
    { WeaponId::Talon, 59 }, // Slaughter
    { WeaponId::Talon, 42 }, // Blue Steel
    { WeaponId::Talon, 5 }, // Forest DDPAT
    { WeaponId::Talon, 43 }, // Stained
    { WeaponId::Talon, 175 }, // Scorched
    { WeaponId::Talon, 77 }, // Boreal Forest
    { WeaponId::Talon, 0 }, // (Vanilla)

    { WeaponId::Ursus, 38 }, // Fade
    { WeaponId::Ursus, 143 }, // Urban Masked
    { WeaponId::Ursus, 72 }, // Safari Mesh
    { WeaponId::Ursus, 12 }, // Crimson Web
    { WeaponId::Ursus, 735 }, // Night Stripe
    { WeaponId::Ursus, 44 }, // Case Hardened
    { WeaponId::Ursus, 59 }, // Slaughter
    { WeaponId::Ursus, 42 }, // Blue Steel
    { WeaponId::Ursus, 5 }, // Forest DDPAT
    { WeaponId::Ursus, 43 }, // Stained
    { WeaponId::Ursus, 175 }, // Scorched
    { WeaponId::Ursus, 77 }, // Boreal Forest
    { WeaponId::Ursus, 0 }, // (Vanilla)
});

constexpr auto clutchGloves = std::to_array<RareSpecialItem>({
    { WeaponId::GloveSlick, 10041 }, // King Snake
    { WeaponId::GloveSlick, 10042 }, // Imperial Plaid
    { WeaponId::GloveSlick, 10043 }, // Overtake
    { WeaponId::GloveSlick, 10044 }, // Racing Green

    { WeaponId::GloveLeatherWrap, 10053 }, // Cobalt Skulls
    { WeaponId::GloveLeatherWrap, 10054 }, // Overprint
    { WeaponId::GloveLeatherWrap, 10056 }, // Arboreal
    { WeaponId::GloveLeatherWrap, 10055 }, // Duct Tape

    { WeaponId::GloveHydra, 10060 }, // Case Hardened
    { WeaponId::GloveHydra, 10057 }, // Emerald
    { WeaponId::GloveHydra, 10059 }, // Rattler
    { WeaponId::GloveHydra, 10058 }, // Mangrove

    { WeaponId::GloveMotorcycle, 10049 }, // POW!
    { WeaponId::GloveMotorcycle, 10052 }, // Polygon
    { WeaponId::GloveMotorcycle, 10050 }, // Turtle
    { WeaponId::GloveMotorcycle, 10051 }, // Transport

    { WeaponId::GloveSpecialist, 10064 }, // Mogul
    { WeaponId::GloveSpecialist, 10061 }, // Crimson Web
    { WeaponId::GloveSpecialist, 10063 }, // Fade
    { WeaponId::GloveSpecialist, 10062 }, // Buckshot

    { WeaponId::GloveSporty, 10048 }, // Vice
    { WeaponId::GloveSporty, 10047 }, // Omega
    { WeaponId::GloveSporty, 10045 }, // Amphibious
    { WeaponId::GloveSporty, 10046 }, // Bronze Morph
});

struct CrateRareSpecialItems {
    WeaponId crateWeaponID;
    std::span<const RareSpecialItem> items;
};

constexpr auto crateRareSpecialItems = std::to_array<CrateRareSpecialItems>({
    { WeaponId::ChromaCase, chromaKnives },
    { WeaponId::Chroma2Case, chromaKnives },
    { WeaponId::Chroma3Case, chromaKnives },
    { WeaponId::ClutchCase, clutchGloves },
    { WeaponId::HorizonCase, horizonKnives },
    { WeaponId::DangerZoneCase, horizonKnives },
    { WeaponId::PrismaCase, prismaKnives },
    { WeaponId::CS20Case, cs20Knives },
    { WeaponId::ShatteredWebCase, shatteredWebKnives },
    { WeaponId::Prisma2Case, prismaKnives },
    { WeaponId::FractureCase, shatteredWebKnives },
    { WeaponId::OperationBrokenFangCase, operationBrokenFangGloves },
    { WeaponId::SnakebiteCase, operationBrokenFangGloves },
    { WeaponId::OperationRiptideCase, operationRiptideKnives },
    { WeaponId::DreamsAndNightmaresCase, operationRiptideKnives }
});

[[nodiscard]] constexpr std::span<const RareSpecialItem> getRareSpecialItems(WeaponId crateWeaponID)
{
    if (const auto it = std::ranges::find(crateRareSpecialItems, crateWeaponID, &CrateRareSpecialItems::crateWeaponID); it != crateRareSpecialItems.end())
        return it->items;
    return {};
}

[[nodiscard]] static EconRarity getRandomRarity(StaticData::EconRarities rarities)
{
    if (const auto rate = std::ranges::find(dropRates, rarities, &DropRate::rarities); rate != dropRates.end()) {
        const auto rolledNumber = Helpers::random((std::numeric_limits<DropRate::T>::min)(), (std::numeric_limits<DropRate::T>::max)());
        return rate->mapToRarity(rolledNumber);
    }
    return EconRarity::Default;
}

[[nodiscard]] const game_items::Item& getRandomItemIndexFromContainer(WeaponId weaponID, const StaticData::Case& container) noexcept
{
    assert(container.hasLoot());

    const auto rareSpecialItems = getRareSpecialItems(weaponID);
    auto rarities = container.rarities;

    if (!rareSpecialItems.empty())
        rarities.set(EconRarity::Gold);

    if (const auto rarity = getRandomRarity(rarities); rarity != EconRarity::Default) {
        if (rarity == EconRarity::Gold) {
            const auto& randomRareSpecialItem = rareSpecialItems[Helpers::random<std::size_t>(0u, rareSpecialItems.size() - 1u)];
            if (const auto item = StaticData::lookup().findItem(randomRareSpecialItem.weaponID, randomRareSpecialItem.paintKit); item.has_value())
                return *item;
        } else {
            const auto loot = StaticData::getCrateLootOfRarity(container, rarity);
            return loot[Helpers::random<std::size_t>(0u, loot.size() - 1u)];
        }
    }

    std::span<const std::reference_wrapper<const game_items::Item>> loot = StaticData::getCrateLoot(container);
    assert(!loot.empty());
    return loot[Helpers::random<std::size_t>(0u, loot.size() - 1u)];
}

std::pair<const game_items::Item&, std::size_t> ItemGenerator::generateItemFromContainer(const InventoryItem& caseItem) noexcept
{
    assert(caseItem.isCase());

    const auto& caseData = StaticData::getCase(caseItem.get());
    assert(caseData.hasLoot());

    const auto& unlockedItem = getRandomItemIndexFromContainer(caseItem.get().getWeaponID(), caseData);
    std::size_t dynamicDataIdx = Inventory::InvalidDynamicDataIdx;

    if (caseData.willProduceStatTrak && unlockedItem.isMusic()) {
        DynamicMusicData dynamicData;
        dynamicData.statTrak = 0;
        dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (unlockedItem.isSkin()) {
        DynamicSkinData dynamicData;
        const auto& staticData = StaticData::lookup().getStorage().getPaintKit(unlockedItem);
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, generateWear());
        dynamicData.seed = Helpers::random(1, 1000);

        if (StaticData::isSouvenirPackage(caseItem.get())) {
            dynamicData.tournamentID = caseData.souvenirPackageTournamentID;
            const auto& souvenir = Inventory::dynamicSouvenirPackageData(caseItem.getDynamicDataIndex());
            dynamicData.tournamentStage = souvenir.tournamentStage;
            dynamicData.tournamentTeam1 = souvenir.tournamentTeam1;
            dynamicData.tournamentTeam2 = souvenir.tournamentTeam2;
            dynamicData.proPlayer = souvenir.proPlayer;
            dynamicData.stickers = generateSouvenirStickers(unlockedItem.getWeaponID(), caseData.souvenirPackageTournamentID, caseData.tournamentMap, dynamicData.tournamentStage, dynamicData.tournamentTeam1, dynamicData.tournamentTeam2, dynamicData.proPlayer);
        } else if (Helpers::random(0, 9) == 0) {
            dynamicData.statTrak = 0;
        }

        dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
    }

    return std::make_pair(std::cref(unlockedItem), dynamicDataIdx);
}

[[nodiscard]] static std::span<const Match> getTournamentMatches(std::uint32_t tournamentID) noexcept
{
    if (const auto it = std::ranges::lower_bound(tournaments, tournamentID, {}, &Tournament::tournamentID); it != tournaments.end() && it->tournamentID == tournamentID)
        return it->matches;
    return {};
}

constexpr auto operator<=>(TournamentMap a, TournamentMap b) noexcept
{
    return static_cast<std::underlying_type_t<TournamentMap>>(a) <=> static_cast<std::underlying_type_t<TournamentMap>>(b);
}

[[nodiscard]] static std::span<const Match> filterMatchesToMap(std::span<const Match> matches, TournamentMap map) noexcept
{
    if (map == TournamentMap::None)
        return matches;

    assert(std::ranges::is_sorted(matches, {}, &Match::map));

    struct Comp {
        bool operator()(TournamentMap map, const Match& match) const noexcept { return map < match.map; }
        bool operator()(const Match& match, TournamentMap map) const noexcept { return match.map < map; }
    };

    // not using std::ranges::equal_range() here because clang 12 on linux doesn't support it yet
    if (const auto [begin, end] = std::equal_range(matches.begin(), matches.end(), map, Comp{}); begin != end)
        return { begin, end };

    return {};
}

[[nodiscard]] static auto generateSouvenirPackageData(const StaticData::Case& caseData) noexcept
{
    DynamicSouvenirPackageData dynamicData;

    if (const auto matches = getTournamentMatches(caseData.souvenirPackageTournamentID); !matches.empty()) {
        if (const auto matchesOnMap = filterMatchesToMap(matches, caseData.tournamentMap); !matchesOnMap.empty()) {
            const auto& randomMatch = matchesOnMap[Helpers::random(static_cast<std::size_t>(0), matchesOnMap.size() - 1)];
            dynamicData.tournamentStage = randomMatch.stage;
            dynamicData.tournamentTeam1 = randomMatch.team1;
            dynamicData.tournamentTeam2 = randomMatch.team2;
            dynamicData.proPlayer = randomMatch.getRandomMVP();
        }
    }

    return dynamicData;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept
{
#ifdef _WIN32
    return _mkgmtime(&tm);
#else
    return timegm(&tm);
#endif
}

[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept
{
    assert(year >= 1900);
    std::tm tm{};
    tm.tm_mday = 1;
    tm.tm_year = year - 1900;
    return tmToUTCTimestamp(tm);
}

[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept
{
    assert(year >= 1900);
    std::tm tm{};
    tm.tm_sec = 59;
    tm.tm_min = 59;
    tm.tm_hour = 23;
    tm.tm_mday = 31;
    tm.tm_mon = 12 - 1;
    tm.tm_year = year - 1900;
    return tmToUTCTimestamp(tm);
}

[[nodiscard]] static std::pair<std::time_t, std::time_t> clampTimespanToNow(std::time_t min, std::time_t max) noexcept
{
    const auto now = std::time(nullptr);
    return std::make_pair((std::min)(min, now), (std::min)(max, now));
}

[[nodiscard]] static std::uint32_t getRandomDateTimestampOfYear(std::uint16_t year) noexcept
{
    const auto [min, max] = clampTimespanToNow(getStartOfYearTimestamp(year), getEndOfYearTimestamp(year));
    return static_cast<std::uint32_t>(Helpers::random(min, max));
}

std::size_t ItemGenerator::createDefaultDynamicData(const game_items::Item& item) noexcept
{
    std::size_t index = Inventory::InvalidDynamicDataIdx;

    if (item.isSkin()) {
        const auto& staticData = StaticData::lookup().getStorage().getPaintKit(item);
        DynamicSkinData dynamicData;
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
        dynamicData.seed = Helpers::random(1, 1000);

        if (Helpers::isMP5LabRats(item.getWeaponID(), StaticData::lookup().getStorage().getPaintKit(item).id))
            dynamicData.stickers[3].stickerID = 28;

        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isGloves()) {
        const auto& staticData = StaticData::lookup().getStorage().getPaintKit(item);
        DynamicGloveData dynamicData;
        dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
        dynamicData.seed = Helpers::random(1, 1000);
        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isAgent()) {
        index = Inventory::emplaceDynamicData(DynamicAgentData{});
    } else if (item.isMusic()) {
        index = Inventory::emplaceDynamicData(DynamicMusicData{});
    } else if (item.isCase()) {
        if (const auto& staticData = StaticData::getCase(item); StaticData::isSouvenirPackage(item))
            index = Inventory::emplaceDynamicData(generateSouvenirPackageData(staticData));
    } else if (item.isServiceMedal()) {
        DynamicServiceMedalData dynamicData;
        dynamicData.issueDateTimestamp = getRandomDateTimestampOfYear(StaticData::lookup().getStorage().getServiceMedalYear(item));
        index = Inventory::emplaceDynamicData(std::move(dynamicData));
    } else if (item.isTournamentCoin()) {
        index = Inventory::emplaceDynamicData(DynamicTournamentCoinData{});
    }

    return index;
}

[[nodiscard]] static const Match* findTournamentMatch(std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2) noexcept
{
    const auto matchesOnMap = filterMatchesToMap(getTournamentMatches(tournamentID), map);
    if (matchesOnMap.empty())
        return nullptr;

    const auto match = std::ranges::find_if(matchesOnMap, [stage, team1, team2](const auto& match) { return match.stage == stage && match.team1 == team1 && match.team2 == team2; });
    return (match != matchesOnMap.end() ? &*match : nullptr);
}

[[nodiscard]] static std::uint8_t getNumberOfSupportedStickerSlots(WeaponId weaponID) noexcept
{
    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(weaponID))
        return static_cast<std::uint8_t>(std::clamp(def->getNumberOfSupportedStickerSlots(), 0, 5));
    return 0;
}

[[nodiscard]] static std::array<StickerConfig, 5> generateSouvenirStickers(WeaponId weaponID, std::uint32_t tournamentID, TournamentMap map, TournamentStage stage, TournamentTeam team1, TournamentTeam team2, ProPlayer player) noexcept
{
    std::array<StickerConfig, 5> stickers;

    stickers[0].stickerID = StaticData::lookup().findTournamentEventStickerID(tournamentID);

    if (tournamentID != 1) {
        stickers[1].stickerID = StaticData::lookup().findTournamentTeamGoldStickerID(tournamentID, team1);
        stickers[2].stickerID = StaticData::lookup().findTournamentTeamGoldStickerID(tournamentID, team2);

        if (const auto match = findTournamentMatch(tournamentID, map, stage, team1, team2); match && match->hasMVPs())
            stickers[3].stickerID = StaticData::lookup().findTournamentPlayerGoldStickerID(tournamentID, static_cast<int>(player));
        else if (tournamentID >= 18) // starting with PGL Stockholm 2021
            stickers[3].stickerID = StaticData::getTournamentMapGoldStickerID(map);
    }

    std::mt19937 gen{ std::random_device{}() };
    std::shuffle(stickers.begin(), stickers.begin() + getNumberOfSupportedStickerSlots(weaponID), gen);

    return stickers;
}
