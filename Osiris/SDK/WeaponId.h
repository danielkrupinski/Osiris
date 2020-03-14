#pragma once

enum class WeaponId : short {
    Deagle = 1,
    Elite,
    Fiveseven,
    Glock,
    Ak47 = 7,
    Aug,
    Awp,
    Famas,
    G3SG1,
    GalilAr = 13,
    M249,
    M4A1 = 16,
    Mac10,
    P90 = 19,
    Mp5sd = 23,
    Ump45,
    Xm1014,
    Bizon,
    Mag7,
    Negev,
    Sawedoff,
    Tec9,
    Taser,
    Hkp2000,
    Mp7,
    Mp9,
    Nova,
    P250,
    Scar20 = 38,
    Sg553,
    Ssg08,
    Knife = 42,
    Flashbang = 43,
    HeGrenade = 44,
    SmokeGrenade = 45,
    Molotov = 46,
    Decoy = 47,
    IncGrenade = 48,
    GoldenKnife,
    KnifeT = 59,
    M4a1_s = 60,
    Usp_s,
    Cz75a = 63,
    Revolver,
    GhostKnife = 80,
    Bayonet = 500,
    KnifeCss = 503,
    Flip = 505,
    Gut,
    Karambit,
    M9Bayonet,
    Huntsman,
    Falchion = 512,
    Bowie = 514,
    Butterfly,
    Daggers,
    Cord,
    Canis,
    Ursus,
    Navaja,
    KnifeOutdoor,
    Stiletto,
    Talon,
    Skeleton = 525,
    GloveStuddedBloodhound = 5027,
    GloveT,
    GloveCT,
    GloveSporty,
    GloveSlick,
    GloveLeatherWrap,
    GloveMotorcycle,
    GloveSpecialist,
    GloveHydra
};

constexpr int getWeaponIndex(WeaponId weaponId) noexcept
{
    switch (weaponId) {
    default: return 0;

    case WeaponId::Glock: return 1;
    case WeaponId::Hkp2000: return 2;
    case WeaponId::Usp_s: return 3;
    case WeaponId::Elite: return 4;
    case WeaponId::P250: return 5;
    case WeaponId::Tec9: return 6;
    case WeaponId::Fiveseven: return 7;
    case WeaponId::Cz75a: return 8;
    case WeaponId::Deagle: return 9;
    case WeaponId::Revolver: return 10;

    case WeaponId::Nova: return 11;
    case WeaponId::Xm1014: return 12;
    case WeaponId::Sawedoff: return 13;
    case WeaponId::Mag7: return 14;
    case WeaponId::M249: return 15;
    case WeaponId::Negev: return 16;

    case WeaponId::Mac10: return 17;
    case WeaponId::Mp9: return 18;
    case WeaponId::Mp7: return 19;
    case WeaponId::Mp5sd: return 20;
    case WeaponId::Ump45: return 21;
    case WeaponId::P90: return 22;
    case WeaponId::Bizon: return 23;

    case WeaponId::GalilAr: return 24;
    case WeaponId::Famas: return 25;
    case WeaponId::Ak47: return 26;
    case WeaponId::M4A1: return 27;
    case WeaponId::M4a1_s: return 28;
    case WeaponId::Ssg08: return 29;
    case WeaponId::Sg553: return 30;
    case WeaponId::Aug: return 31;
    case WeaponId::Awp: return 32;
    case WeaponId::G3SG1: return 33;
    case WeaponId::Scar20: return 34;

    case WeaponId::Taser: return 39;
    case WeaponId::Flashbang:return 41;
    case WeaponId::Decoy:return 42;
    case WeaponId::HeGrenade:return 43;
    case WeaponId::IncGrenade:return 44;
    case WeaponId::Molotov:return 45;
    case WeaponId::SmokeGrenade: return 46;

    case WeaponId::GhostKnife:return 47;
    case WeaponId::GoldenKnife:return 48;
    case WeaponId::Talon:return 49;
    case WeaponId::Stiletto:return 50;
    case WeaponId::Navaja:return 51;
    case WeaponId::Ursus: return 52;
    case WeaponId::Daggers:return 53;
    case WeaponId::Butterfly:return 54;
    case WeaponId::Bowie:return 55;
    case WeaponId::Falchion:return 56;
    case WeaponId::Huntsman:return 57;
    case WeaponId::M9Bayonet: return 58;
    case WeaponId::Karambit:return 59;
    case WeaponId::Gut:return 60;
    case WeaponId::Flip: return 61;
    case WeaponId::Bayonet: return 62;
    }
}
constexpr int getWeaponClass(WeaponId weaponId) noexcept
{
    switch (weaponId) {
    default: return 0;

    case WeaponId::Glock:
    case WeaponId::Hkp2000:
    case WeaponId::Usp_s:
    case WeaponId::Elite:
    case WeaponId::P250:
    case WeaponId::Tec9:
    case WeaponId::Fiveseven:
    case WeaponId::Cz75a:
    case WeaponId::Deagle:
    case WeaponId::Revolver: return 35;

    case WeaponId::Nova:
    case WeaponId::Xm1014:
    case WeaponId::Sawedoff:
    case WeaponId::Mag7:
    case WeaponId::M249:
    case WeaponId::Negev: return 36;

    case WeaponId::Mac10:
    case WeaponId::Mp9:
    case WeaponId::Mp7:
    case WeaponId::Mp5sd:
    case WeaponId::Ump45:
    case WeaponId::P90:
    case WeaponId::Bizon: return 37;

    case WeaponId::GalilAr:
    case WeaponId::Famas:
    case WeaponId::Ak47:
    case WeaponId::M4A1:
    case WeaponId::M4a1_s:
    case WeaponId::Ssg08:
    case WeaponId::Sg553:
    case WeaponId::Aug:
    case WeaponId::Awp:
    case WeaponId::G3SG1:
    case WeaponId::Scar20: return 38;

    case WeaponId::Flashbang:
    case WeaponId::Decoy:
    case WeaponId::HeGrenade:
    case WeaponId::IncGrenade:
    case WeaponId::Molotov:
    case WeaponId::SmokeGrenade: return 40;

    case WeaponId::Knife: //42
    case WeaponId::GoldenKnife: //49
    case WeaponId::KnifeT: //59
    case WeaponId::GhostKnife: //80
    case WeaponId::Bayonet: //500
    case WeaponId::KnifeCss: //503
    case WeaponId::Flip: //505
    case WeaponId::Gut: //506
    case WeaponId::Karambit: //507
    case WeaponId::M9Bayonet: //508
    case WeaponId::Huntsman: //509
    case WeaponId::Falchion: //512
    case WeaponId::Bowie: //514
    case WeaponId::Butterfly: //515
    case WeaponId::Daggers: //516
    case WeaponId::Cord: //517
    case WeaponId::Canis: //518
    case WeaponId::Ursus: //519
    case WeaponId::Navaja: //520
    case WeaponId::KnifeOutdoor: //521
    case WeaponId::Stiletto: //522
    case WeaponId::Talon: //523
    case WeaponId::Skeleton: return 63; //525 
    }
}
