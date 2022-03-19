#pragma once

enum class WeaponId : short {
    None = 0,
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
    ZoneRepulsor,
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
    Shield,
    Scar20,
    Sg553,
    Ssg08,
    GoldenKnife,
    Knife,
    Flashbang = 43,
    HeGrenade,
    SmokeGrenade,
    Molotov,
    Decoy,
    IncGrenade,
    C4,
    Healthshot = 57,
    KnifeT = 59,
    M4a1_s,
    Usp_s,
    Cz75a = 63,
    Revolver,
    TaGrenade = 68,
    Axe = 75,
    Hammer,
    Spanner = 78,
    GhostKnife = 80,
    Firebomb,
    Diversion,
    FragGrenade,
    Snowball,
    BumpMine,
    Bayonet = 500,
    ClassicKnife = 503,
    Flip = 505,
    Gut,
    Karambit,
    M9Bayonet,
    Huntsman,
    Falchion = 512,
    Bowie = 514,
    Butterfly,
    Daggers,
    Paracord,
    SurvivalKnife,
    Ursus = 519,
    Navaja,
    NomadKnife,
    Stiletto = 522,
    Talon,
    SkeletonKnife = 525,
    NameTag = 1200,
    Sticker = 1209,
    MusicKit = 1314,
    SealedGraffiti = 1348,
    Graffiti = 1349,
    OperationHydraPass = 1352,
    CSGOWeaponCase = 4001,
    Esports2013Case = 4002,
    OperationBravoCase = 4003,
    CSGOWeaponCase2 = 4004,
    Esports2013WinterCase = 4005,
    WinterOffensiveCase = 4009,
    CSGOWeaponCase3 = 4010,
    OperationPhoenixCase = 4011,
    HuntsmanCase = 4017,
    OperationBreakoutCase = 4018,
    Esports2014SummerCase = 4019,
    OperationVanguardCase = 4029,
    ChromaCase = 4061,
    Chroma2Case = 4089,
    FalchionCase = 4091,
    ShadowCase = 4138,
    RevolverCase = 4186,
    OperationWildfireCase = 4187,
    Chroma3Case = 4233,
    GammaCase = 4236,
    Gamma2Case = 4281,
    GloveCase = 4288,
    SpectrumCase = 4351,
    OperationHydraCase = 4352,
    BronzeOperationHydraCoin = 4353,
    Spectrum2Case = 4403,
    ClutchCase = 4471,
    HorizonCase = 4482,
    DangerZoneCase = 4548,
    PrismaCase = 4598,
    Patch = 4609,
    ShatteredWebCase = 4620,
    Berlin2019SouvenirToken = 4628,
    CS20Case = 4669,
    Prisma2Case = 4695,
    FractureCase = 4698,
    GloveStuddedBrokenfang = 4725,
    OperationBrokenFangCase = 4717,
    SnakebiteCase = 4747,
    OperationRiptideCase = 4790,
    Stockholm2021SouvenirToken = 4802,
    DreamsAndNightmaresCase = 4818,
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
    }
}
