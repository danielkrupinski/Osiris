#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <ctime>
#include <random>
#include <span>

#include <Helpers.h>
#include "../../Memory.h"
#include "ItemGenerator.h"

#include <CSGO/ItemSchema.h>
#include "TournamentMatches.h"
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/Inventory/Structs.h>
#include <InventoryChanger/GameIntegration/Misc.h>

#include "AttributeGenerator.h"
#include "DefaultGenerator.h"
#include "DropGenerator.h"

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

    constexpr DropRate(inventory_changer::EconRarities rarities, const std::array<float, MaxDistinctRarities>& chances)
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

    inventory_changer::EconRarities rarities;
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
    short paintKit;
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

constexpr auto spectrumKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Bowie, 415 }, // Doppler Ruby
    { WeaponId::Bowie, 416 }, // Doppler Sapphire
    { WeaponId::Bowie, 417 }, // Doppler Black Pearl
    { WeaponId::Bowie, 418 }, // Doppler Phase 1
    { WeaponId::Bowie, 419 }, // Doppler Phase 2
    { WeaponId::Bowie, 420 }, // Doppler Phase 3
    { WeaponId::Bowie, 421 }, // Doppler Phase 4
    { WeaponId::Bowie, 413 }, // Marble Fade
    { WeaponId::Bowie, 409 }, // Tiger Tooth
    { WeaponId::Bowie, 98 }, // Ultraviolet
    { WeaponId::Bowie, 411 }, // Damascus Steel
    { WeaponId::Bowie, 414 }, // Rust Coat

    { WeaponId::Butterfly, 415 }, // Doppler Ruby
    { WeaponId::Butterfly, 619 }, // Doppler Sapphire
    { WeaponId::Butterfly, 617 }, // Doppler Black Pearl
    { WeaponId::Butterfly, 418 }, // Doppler Phase 1
    { WeaponId::Butterfly, 618 }, // Doppler Phase 2
    { WeaponId::Butterfly, 420 }, // Doppler Phase 3
    { WeaponId::Butterfly, 421 }, // Doppler Phase 4
    { WeaponId::Butterfly, 413 }, // Marble Fade
    { WeaponId::Butterfly, 409 }, // Tiger Tooth
    { WeaponId::Butterfly, 98 }, // Ultraviolet
    { WeaponId::Butterfly, 411 }, // Damascus Steel
    { WeaponId::Butterfly, 414 }, // Rust Coat

    { WeaponId::Falchion, 415 }, // Doppler Ruby
    { WeaponId::Falchion, 416 }, // Doppler Sapphire
    { WeaponId::Falchion, 417 }, // Doppler Black Pearl
    { WeaponId::Falchion, 418 }, // Doppler Phase 1
    { WeaponId::Falchion, 419 }, // Doppler Phase 2
    { WeaponId::Falchion, 420 }, // Doppler Phase 3
    { WeaponId::Falchion, 421 }, // Doppler Phase 4
    { WeaponId::Falchion, 413 }, // Marble Fade
    { WeaponId::Falchion, 409 }, // Tiger Tooth
    { WeaponId::Falchion, 621 }, // Ultraviolet
    { WeaponId::Falchion, 411 }, // Damascus Steel
    { WeaponId::Falchion, 414 }, // Rust Coat

    { WeaponId::Huntsman, 415 }, // Doppler Ruby
    { WeaponId::Huntsman, 416 }, // Doppler Sapphire
    { WeaponId::Huntsman, 417 }, // Doppler Black Pearl
    { WeaponId::Huntsman, 418 }, // Doppler Phase 1
    { WeaponId::Huntsman, 419 }, // Doppler Phase 2
    { WeaponId::Huntsman, 420 }, // Doppler Phase 3
    { WeaponId::Huntsman, 421 }, // Doppler Phase 4
    { WeaponId::Huntsman, 413 }, // Marble Fade
    { WeaponId::Huntsman, 409 }, // Tiger Tooth
    { WeaponId::Huntsman, 620 }, // Ultraviolet
    { WeaponId::Huntsman, 411 }, // Damascus Steel
    { WeaponId::Huntsman, 414 }, // Rust Coat

    { WeaponId::Daggers, 415 }, // Doppler Ruby
    { WeaponId::Daggers, 619 }, // Doppler Sapphire
    { WeaponId::Daggers, 617 }, // Doppler Black Pearl
    { WeaponId::Daggers, 418 }, // Doppler Phase 1
    { WeaponId::Daggers, 618 }, // Doppler Phase 2
    { WeaponId::Daggers, 420 }, // Doppler Phase 3
    { WeaponId::Daggers, 421 }, // Doppler Phase 4
    { WeaponId::Daggers, 413 }, // Marble Fade
    { WeaponId::Daggers, 409 }, // Tiger Tooth
    { WeaponId::Daggers, 98 }, // Ultraviolet
    { WeaponId::Daggers, 411 }, // Damascus Steel
    { WeaponId::Daggers, 414 }, // Rust Coat
});

constexpr auto gloveCaseGloves = std::to_array<RareSpecialItem>({
    { WeaponId::GloveStuddedBloodhound, 10007 }, // Snakebite
    { WeaponId::GloveStuddedBloodhound, 10008 }, // Bronzed
    { WeaponId::GloveStuddedBloodhound, 10006 }, // Charred
    { WeaponId::GloveStuddedBloodhound, 10039 }, // Guerrilla

    { WeaponId::GloveSlick, 10016 }, // Crimson Weave
    { WeaponId::GloveSlick, 10015 }, // Convoy
    { WeaponId::GloveSlick, 10013 }, // Lunar Weave
    { WeaponId::GloveSlick, 10040 }, // Diamondback

    { WeaponId::GloveLeatherWrap, 10021 }, // Slaughter
    { WeaponId::GloveLeatherWrap, 10009 }, // Leather
    { WeaponId::GloveLeatherWrap, 10036 }, // Badlands
    { WeaponId::GloveLeatherWrap, 10010 }, // Spruce DDPAT

    { WeaponId::GloveMotorcycle, 10026 }, // Spearmint
    { WeaponId::GloveMotorcycle, 10028 }, // Cool Mint
    { WeaponId::GloveMotorcycle, 10027 }, // Boom!
    { WeaponId::GloveMotorcycle, 10024 }, // Eclipse

    { WeaponId::GloveSpecialist, 10033 }, // Crimson Kimono
    { WeaponId::GloveSpecialist, 10034 }, // Emerald Web
    { WeaponId::GloveSpecialist, 10035 }, // Foundation
    { WeaponId::GloveSpecialist, 10030 }, // Forest DDPAT

    { WeaponId::GloveSporty, 10037 }, // Pandora's Box
    { WeaponId::GloveSporty, 10038 }, // Hedge Maze
    { WeaponId::GloveSporty, 10018 }, // Superconductor
    { WeaponId::GloveSporty, 10019 }, // Arid
});

constexpr auto gammaKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Bayonet, 568 }, // Gamma Doppler Emerald
    { WeaponId::Bayonet, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Bayonet, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Bayonet, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Bayonet, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Bayonet, 558 }, // Lore
    { WeaponId::Bayonet, 573 }, // Autotronic
    { WeaponId::Bayonet, 563 }, // Black Laminate
    { WeaponId::Bayonet, 578 }, // Bright Water
    { WeaponId::Bayonet, 580 }, // Freehand

    { WeaponId::Flip, 568 }, // Gamma Doppler Emerald
    { WeaponId::Flip, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Flip, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Flip, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Flip, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Flip, 559 }, // Lore
    { WeaponId::Flip, 574 }, // Autotronic
    { WeaponId::Flip, 564 }, // Black Laminate
    { WeaponId::Flip, 578 }, // Bright Water
    { WeaponId::Flip, 580 }, // Freehand

    { WeaponId::Gut, 568 }, // Gamma Doppler Emerald
    { WeaponId::Gut, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Gut, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Gut, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Gut, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Gut, 560 }, // Lore
    { WeaponId::Gut, 575 }, // Autotronic
    { WeaponId::Gut, 565 }, // Black Laminate
    { WeaponId::Gut, 578 }, // Bright Water
    { WeaponId::Gut, 580 }, // Freehand

    { WeaponId::Karambit, 568 }, // Gamma Doppler Emerald
    { WeaponId::Karambit, 569 }, // Gamma Doppler Phase 1
    { WeaponId::Karambit, 570 }, // Gamma Doppler Phase 2
    { WeaponId::Karambit, 571 }, // Gamma Doppler Phase 3
    { WeaponId::Karambit, 572 }, // Gamma Doppler Phase 4
    { WeaponId::Karambit, 561 }, // Lore
    { WeaponId::Karambit, 576 }, // Autotronic
    { WeaponId::Karambit, 566 }, // Black Laminate
    { WeaponId::Karambit, 578 }, // Bright Water
    { WeaponId::Karambit, 582 }, // Freehand

    { WeaponId::M9Bayonet, 568 }, // Gamma Doppler Emerald
    { WeaponId::M9Bayonet, 569 }, // Gamma Doppler Phase 1
    { WeaponId::M9Bayonet, 570 }, // Gamma Doppler Phase 2
    { WeaponId::M9Bayonet, 571 }, // Gamma Doppler Phase 3
    { WeaponId::M9Bayonet, 572 }, // Gamma Doppler Phase 4
    { WeaponId::M9Bayonet, 562 }, // Lore
    { WeaponId::M9Bayonet, 577 }, // Autotronic
    { WeaponId::M9Bayonet, 567 }, // Black Laminate
    { WeaponId::M9Bayonet, 579 }, // Bright Water
    { WeaponId::M9Bayonet, 581 }, // Freehand
});

constexpr auto operationWildfireKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Bowie, 38 }, // Fade
    { WeaponId::Bowie, 59 }, // Slaughter
    { WeaponId::Bowie, 40 }, // Night
    { WeaponId::Bowie, 12 }, // Crimson Web
    { WeaponId::Bowie, 44 }, // Case Hardened
    { WeaponId::Bowie, 43 }, // Stained
    { WeaponId::Bowie, 0 }, // (Vanilla)
    { WeaponId::Bowie, 42 }, // Blue Steel
    { WeaponId::Bowie, 77 }, // Boreal Forest
    { WeaponId::Bowie, 143 }, // Urban Masked
    { WeaponId::Bowie, 5 }, // Forest DDPAT
    { WeaponId::Bowie, 175 }, // Scorched
    { WeaponId::Bowie, 72 }, // Safari Mesh
});

constexpr auto armsDealKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Bayonet, 38 }, // Fade
    { WeaponId::Bayonet, 0 }, // (Vanilla)
    { WeaponId::Bayonet, 59 }, // Slaughter
    { WeaponId::Bayonet, 12 }, // Crimson Web
    { WeaponId::Bayonet, 44 }, // Case Hardened
    { WeaponId::Bayonet, 42 }, // Blue Steel
    { WeaponId::Bayonet, 43 }, // Stained
    { WeaponId::Bayonet, 40 }, // Night
    { WeaponId::Bayonet, 175 }, // Scorched
    { WeaponId::Bayonet, 5 }, // Forest DDPAT
    { WeaponId::Bayonet, 143 }, // Urban Masked
    { WeaponId::Bayonet, 77 }, // Boreal Forest
    { WeaponId::Bayonet, 72 }, // Safari Mesh

    { WeaponId::Flip, 38 }, // Fade
    { WeaponId::Flip, 0 }, // (Vanilla)
    { WeaponId::Flip, 59 }, // Slaughter
    { WeaponId::Flip, 12 }, // Crimson Web
    { WeaponId::Flip, 44 }, // Case Hardened
    { WeaponId::Flip, 42 }, // Blue Steel
    { WeaponId::Flip, 43 }, // Stained
    { WeaponId::Flip, 40 }, // Night
    { WeaponId::Flip, 175 }, // Scorched
    { WeaponId::Flip, 5 }, // Forest DDPAT
    { WeaponId::Flip, 143 }, // Urban Masked
    { WeaponId::Flip, 77 }, // Boreal Forest
    { WeaponId::Flip, 72 }, // Safari Mesh

    { WeaponId::Gut, 38 }, // Fade
    { WeaponId::Gut, 0 }, // (Vanilla)
    { WeaponId::Gut, 59 }, // Slaughter
    { WeaponId::Gut, 12 }, // Crimson Web
    { WeaponId::Gut, 44 }, // Case Hardened
    { WeaponId::Gut, 42 }, // Blue Steel
    { WeaponId::Gut, 43 }, // Stained
    { WeaponId::Gut, 40 }, // Night
    { WeaponId::Gut, 175 }, // Scorched
    { WeaponId::Gut, 5 }, // Forest DDPAT
    { WeaponId::Gut, 143 }, // Urban Masked
    { WeaponId::Gut, 77 }, // Boreal Forest
    { WeaponId::Gut, 72 }, // Safari Mesh

    { WeaponId::Karambit, 38 }, // Fade
    { WeaponId::Karambit, 0 }, // (Vanilla)
    { WeaponId::Karambit, 59 }, // Slaughter
    { WeaponId::Karambit, 12 }, // Crimson Web
    { WeaponId::Karambit, 44 }, // Case Hardened
    { WeaponId::Karambit, 42 }, // Blue Steel
    { WeaponId::Karambit, 43 }, // Stained
    { WeaponId::Karambit, 40 }, // Night
    { WeaponId::Karambit, 175 }, // Scorched
    { WeaponId::Karambit, 5 }, // Forest DDPAT
    { WeaponId::Karambit, 143 }, // Urban Masked
    { WeaponId::Karambit, 77 }, // Boreal Forest
    { WeaponId::Karambit, 72 }, // Safari Mesh

    { WeaponId::M9Bayonet, 38 }, // Fade
    { WeaponId::M9Bayonet, 0 }, // (Vanilla)
    { WeaponId::M9Bayonet, 59 }, // Slaughter
    { WeaponId::M9Bayonet, 12 }, // Crimson Web
    { WeaponId::M9Bayonet, 44 }, // Case Hardened
    { WeaponId::M9Bayonet, 42 }, // Blue Steel
    { WeaponId::M9Bayonet, 43 }, // Stained
    { WeaponId::M9Bayonet, 40 }, // Night
    { WeaponId::M9Bayonet, 175 }, // Scorched
    { WeaponId::M9Bayonet, 5 }, // Forest DDPAT
    { WeaponId::M9Bayonet, 143 }, // Urban Masked
    { WeaponId::M9Bayonet, 77 }, // Boreal Forest
    { WeaponId::M9Bayonet, 72 }, // Safari Mesh
});

constexpr auto shadowCaseKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Daggers, 38 }, // Fade
    { WeaponId::Daggers, 0 }, // (Vanilla)
    { WeaponId::Daggers, 59 }, // Slaughter
    { WeaponId::Daggers, 12 }, // Crimson Web
    { WeaponId::Daggers, 44 }, // Case Hardened
    { WeaponId::Daggers, 42 }, // Blue Steel
    { WeaponId::Daggers, 43 }, // Stained
    { WeaponId::Daggers, 40 }, // Night
    { WeaponId::Daggers, 175 }, // Scorched
    { WeaponId::Daggers, 5 }, // Forest DDPAT
    { WeaponId::Daggers, 143 }, // Urban Masked
    { WeaponId::Daggers, 77 }, // Boreal Forest
    { WeaponId::Daggers, 72 }, // Safari Mesh
});

constexpr auto falchionCaseKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Falchion, 38 }, // Fade
    { WeaponId::Falchion, 0 }, // (Vanilla)
    { WeaponId::Falchion, 59 }, // Slaughter
    { WeaponId::Falchion, 12 }, // Crimson Web
    { WeaponId::Falchion, 44 }, // Case Hardened
    { WeaponId::Falchion, 42 }, // Blue Steel
    { WeaponId::Falchion, 43 }, // Stained
    { WeaponId::Falchion, 40 }, // Night
    { WeaponId::Falchion, 175 }, // Scorched
    { WeaponId::Falchion, 5 }, // Forest DDPAT
    { WeaponId::Falchion, 143 }, // Urban Masked
    { WeaponId::Falchion, 77 }, // Boreal Forest
    { WeaponId::Falchion, 72 }, // Safari Mesh
});

constexpr auto operationBreakoutKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Butterfly, 38 }, // Fade
    { WeaponId::Butterfly, 0 }, // (Vanilla)
    { WeaponId::Butterfly, 59 }, // Slaughter
    { WeaponId::Butterfly, 12 }, // Crimson Web
    { WeaponId::Butterfly, 44 }, // Case Hardened
    { WeaponId::Butterfly, 42 }, // Blue Steel
    { WeaponId::Butterfly, 43 }, // Stained
    { WeaponId::Butterfly, 40 }, // Night
    { WeaponId::Butterfly, 175 }, // Scorched
    { WeaponId::Butterfly, 5 }, // Forest DDPAT
    { WeaponId::Butterfly, 143 }, // Urban Masked
    { WeaponId::Butterfly, 77 }, // Boreal Forest
    { WeaponId::Butterfly, 72 }, // Safari Mesh
});

constexpr auto huntsmanCaseKnives = std::to_array<RareSpecialItem>({
    { WeaponId::Huntsman, 38 }, // Fade
    { WeaponId::Huntsman, 0 }, // (Vanilla)
    { WeaponId::Huntsman, 59 }, // Slaughter
    { WeaponId::Huntsman, 12 }, // Crimson Web
    { WeaponId::Huntsman, 44 }, // Case Hardened
    { WeaponId::Huntsman, 42 }, // Blue Steel
    { WeaponId::Huntsman, 43 }, // Stained
    { WeaponId::Huntsman, 40 }, // Night
    { WeaponId::Huntsman, 175 }, // Scorched
    { WeaponId::Huntsman, 5 }, // Forest DDPAT
    { WeaponId::Huntsman, 143 }, // Urban Masked
    { WeaponId::Huntsman, 77 }, // Boreal Forest
    { WeaponId::Huntsman, 72 }, // Safari Mesh
});

struct CrateRareSpecialItems {
    WeaponId crateWeaponID;
    std::span<const RareSpecialItem> items;
};

constexpr auto crateRareSpecialItems = std::to_array<CrateRareSpecialItems>({
    { WeaponId::CSGOWeaponCase, armsDealKnives },
    { WeaponId::Esports2013Case, armsDealKnives },
    { WeaponId::OperationBravoCase, armsDealKnives },
    { WeaponId::CSGOWeaponCase2, armsDealKnives },
    { WeaponId::Esports2013WinterCase, armsDealKnives },
    { WeaponId::WinterOffensiveCase, armsDealKnives },
    { WeaponId::CSGOWeaponCase3, armsDealKnives },
    { WeaponId::OperationPhoenixCase, armsDealKnives },
    { WeaponId::HuntsmanCase, huntsmanCaseKnives },
    { WeaponId::OperationBreakoutCase, operationBreakoutKnives },
    { WeaponId::Esports2014SummerCase, armsDealKnives },
    { WeaponId::OperationVanguardCase, armsDealKnives },
    { WeaponId::ChromaCase, chromaKnives },
    { WeaponId::Chroma2Case, chromaKnives },
    { WeaponId::FalchionCase, falchionCaseKnives },
    { WeaponId::ShadowCase, shadowCaseKnives },
    { WeaponId::RevolverCase, armsDealKnives },
    { WeaponId::OperationWildfireCase, operationWildfireKnives },
    { WeaponId::Chroma3Case, chromaKnives },
    { WeaponId::GammaCase, gammaKnives },
    { WeaponId::Gamma2Case, gammaKnives },
    { WeaponId::GloveCase, gloveCaseGloves },
    { WeaponId::SpectrumCase, spectrumKnives },
    { WeaponId::OperationHydraCase, gloveCaseGloves },
    { WeaponId::Spectrum2Case, spectrumKnives },
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
    { WeaponId::DreamsAndNightmaresCase, operationRiptideKnives },
    { WeaponId::RecoilCase, operationBrokenFangGloves },
    { WeaponId::RevolutionCase, clutchGloves }
});

[[nodiscard]] constexpr std::span<const RareSpecialItem> getRareSpecialItems(WeaponId crateWeaponID)
{
    if (const auto it = std::ranges::find(crateRareSpecialItems, crateWeaponID, &CrateRareSpecialItems::crateWeaponID); it != crateRareSpecialItems.end())
        return it->items;
    return {};
}

[[nodiscard]] static EconRarity getRandomRarity(inventory_changer::EconRarities rarities, Helpers::RandomGenerator& randomGenerator)
{
    if (const auto rate = std::ranges::find(dropRates, rarities, &DropRate::rarities); rate != dropRates.end()) {
        const auto rolledNumber = randomGenerator(std::uniform_int_distribution<DropRate::T>{(std::numeric_limits<DropRate::T>::min)(), (std::numeric_limits<DropRate::T>::max)()});
        return rate->mapToRarity(rolledNumber);
    }
    return EconRarity::Default;
}

namespace inventory_changer
{

[[nodiscard]] const game_items::Item& getRandomItemFromContainer(Helpers::RandomGenerator& randomGenerator, const game_items::Lookup& lookup, const game_items::CrateLootLookup& crateLootLookup, WeaponId weaponID, const game_items::CrateLoot::LootList& lootList) noexcept
{
    const auto rareSpecialItems = getRareSpecialItems(weaponID);
    auto rarities = lootList.rarities;

    if (!rareSpecialItems.empty())
        rarities.set(EconRarity::Gold);

    if (const auto rarity = getRandomRarity(rarities, randomGenerator); rarity != EconRarity::Default) {
        if (rarity == EconRarity::Gold) {
            const auto& randomRareSpecialItem = rareSpecialItems[randomGenerator(std::uniform_int_distribution<std::size_t>{0u, rareSpecialItems.size() - 1u})];
            if (const auto item = lookup.findItem(randomRareSpecialItem.weaponID, randomRareSpecialItem.paintKit))
                return *item;
        } else {
            const auto loot = game_items::getLootOfRarity(crateLootLookup, lootList.crateSeries, rarity);
            return loot[randomGenerator(std::uniform_int_distribution<std::size_t>{0u, loot.size() - 1u})];
        }
    }

    std::span<const std::reference_wrapper<const game_items::Item>> loot = crateLootLookup.getLoot(lootList.crateSeries);
    assert(!loot.empty());
    return loot[randomGenerator(std::uniform_int_distribution<std::size_t>{0u, loot.size() - 1u})];
}

}

[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept
{
    using namespace std::chrono;
    return system_clock::to_time_t(sys_days(1d / January / year));
}

[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept
{
    using namespace std::chrono;
    return system_clock::to_time_t(sys_days(31d / December / year) + 23h + 59min + 59s);
}

namespace inventory_changer
{

[[nodiscard]] inline std::uint8_t getNumberOfSupportedStickerSlots(const Memory& memory, WeaponId weaponID) noexcept
{
    if (const auto def = csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()).getItemDefinitionInterface(weaponID))
        return static_cast<std::uint8_t>(std::clamp(csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getNumberOfSupportedStickerSlots(), 0, 5));
    return 0;
}

struct StickerSlotCountGetter {
public:
    explicit StickerSlotCountGetter(csgo::ItemSchema itemSchema) : itemSchema{ itemSchema } {}

    [[nodiscard]] std::uint8_t operator()(WeaponId weaponId) const
    {
        if (const auto def = itemSchema.getItemDefinitionInterface(weaponId))
            return static_cast<std::uint8_t>(std::clamp(csgo::EconItemDefinition::from(retSpoofGadgets->client, def).getNumberOfSupportedStickerSlots(), 0, 5));
        return 0;
    }

private:
    csgo::ItemSchema itemSchema;
};

std::optional<inventory::Item> ItemGenerator::generateItemFromContainer(const inventory::Item& caseItem, const inventory::Item* crateKey) const noexcept
{
    assert(caseItem.gameItem().isCrate());

    const auto crateSeries = gameItemLookup.getStorage().getCrateSeries(caseItem.gameItem());
    const auto lootList = crateLootLookup.findLootList(crateSeries);
    if (!lootList)
        return std::nullopt;

    const auto& unlockedItem = getRandomItemFromContainer(randomGenerator, gameItemLookup, crateLootLookup, caseItem.gameItem().getWeaponID(), *lootList);
    item_generator::DropGenerator dropGenerator{ gameItemLookup, item_generator::AttributeGenerator{ randomGenerator }, StickerSlotCountGetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) } };
    return inventory::Item{ unlockedItem, { dropGenerator.createCommonProperties(caseItem, crateKey), dropGenerator.createVariantProperties(unlockedItem, caseItem, lootList->willProduceStatTrak) } };
}

inventory::Item::Properties ItemGenerator::createDefaultItemProperties(const game_items::Item& item) const noexcept
{
    item_generator::DefaultGenerator defaultGenerator{ gameItemLookup.getStorage(), item_generator::AttributeGenerator{ randomGenerator } };
    return { defaultGenerator.createCommonProperties(item), defaultGenerator.createVariantProperties(item) };
}

}
