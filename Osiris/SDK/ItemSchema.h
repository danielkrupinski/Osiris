#pragma once

#include <cstddef>

#include "Inconstructible.h"
#include "Pad.h"
#include "Entity.h"
#include "UtlMap.h"
#include "UtlMemory.h"
#include "UtlString.h"
#include "UtlVector.h"
#include "VirtualMethod.h"

#include "../Memory.h"

enum class WeaponId : short;

enum class EconRarity : std::uint8_t {
    Default = 0,
    Gray,
    LightBlue,
    Blue,
    Purple,
    Pink,
    Red,
    Gold
};

struct PaintKit {
    int id;
    UtlString name;
    UtlString description;
    UtlString itemName;
    UtlString sameNameFamilyAggregate;
    UtlString pattern;
    UtlString normal;
    UtlString logoMaterial;
    bool baseDiffuseOverride;
    int rarity;
    PAD(40)
    float wearRemapMin;
    float wearRemapMax;
};

struct StickerKit {
    int id;
    int rarity;
    UtlString name;
    UtlString description;
    UtlString itemName;
    PAD(2 * sizeof(UtlString))
    UtlString inventoryImage;
    int tournamentID;
    int tournamentTeamID;
    int tournamentPlayerID;
};

union AttributeDataUnion {
    float asFloat;
    std::uint32_t asUint32;
    char* asBlobPointer;
};

struct StaticAttrib {
    std::uint16_t defIndex;
    AttributeDataUnion value;
    bool forceGCToGenerate;
};
static_assert(sizeof(StaticAttrib) == WIN32_LINUX(12, 24));

struct EconTool {
    INCONSTRUCTIBLE(EconTool)

    PAD(sizeof(std::uintptr_t))
    const char* typeName;
};

class EconItemDefinition {
public:
    INCONSTRUCTIBLE(EconItemDefinition)

    VIRTUAL_METHOD(WeaponId, getWeaponId, 0, (), (this))
    VIRTUAL_METHOD(const char*, getItemBaseName, 2, (), (this))
    VIRTUAL_METHOD(const char*, getItemTypeName, 3, (), (this))
    VIRTUAL_METHOD(const char*, getInventoryImage, 5, (), (this))
    VIRTUAL_METHOD(const char*, getPlayerDisplayModel, 6, (), (this))
    VIRTUAL_METHOD(const char*, getWorldDisplayModel, 7, (), (this))
    VIRTUAL_METHOD(std::uint8_t, getRarity, 12, (), (this))
    VIRTUAL_METHOD_V(int, getNumberOfSupportedStickerSlots, 44, (), (this))

    std::uint8_t getQuality() noexcept
    {
        return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + WIN32_LINUX(0x2B, 0x4B));
    }

    int getCapabilities() noexcept
    {
        return *reinterpret_cast<int*>(this + WIN32_LINUX(0x148, 0x1F8));
    }

    int getItemType() noexcept
    {
        return *reinterpret_cast<int*>(std::uintptr_t(this) + WIN32_LINUX(0x130, 0x1C8));
    }

    bool isServiceMedal() noexcept
    {
        return getItemType() == 5; /* prestige_coin */
    }

    bool isTournamentCoin() noexcept
    {
        return getItemType() == 8; /* fan_shield */
    }

    const UtlVector<StaticAttrib>& getStaticAttributes() noexcept
    {
        return *reinterpret_cast<const UtlVector<StaticAttrib>*>(std::uintptr_t(this) + WIN32_LINUX(0x30, 0x50));
    }

    std::uint32_t getAttributeValue(std::uint16_t attributeDefinitionIndex) noexcept
    {
        const auto& staticAttributes = getStaticAttributes();
        for (int i = 0; i < staticAttributes.size; ++i) {
            if (staticAttributes[i].defIndex == attributeDefinitionIndex)
                return staticAttributes[i].value.asUint32;
        }
        return 0;
    }

    std::uint32_t getCrateSeriesNumber() noexcept
    {
        return getAttributeValue(68 /* "set supply crate series" */);
    }

    bool hasCrateSeries() noexcept
    {
        return getCrateSeriesNumber() != 0;
    }

    std::uint32_t getTournamentEventID() noexcept
    {
        return getAttributeValue(137 /* "tournament event id" */);
    }

    bool hasTournamentEventID() noexcept
    {
        return getTournamentEventID() != 0;
    }

    std::uint32_t getServiceMedalYear() noexcept
    {
        return getAttributeValue(221 /* "prestige year" */);
    }

    bool isPaintable() noexcept { return getCapabilities() & 1; /* ITEM_CAP_PAINTABLE */ }
    bool isPatchable() noexcept { return getCapabilities() & (1 << 22); /* ITEM_CAP_CAN_PATCH */ }

    const char* getDefinitionName() noexcept
    {
        return *reinterpret_cast<const char**>(this + WIN32_LINUX(0x1DC, 0x2E0));
    }

    EconTool* getEconTool() noexcept
    {
        return *reinterpret_cast<EconTool**>(std::uintptr_t(this) + WIN32_LINUX(0x140, 0x1E8));
    }

    int getLoadoutSlot(Team team) noexcept
    {
        if (team >= Team::None && team <= Team::CT)
            return reinterpret_cast<int*>(std::uintptr_t(this) + WIN32_LINUX(0x28C, 0x3F4))[static_cast<int>(team)];
        return *reinterpret_cast<int*>(std::uintptr_t(this) + WIN32_LINUX(0x268, 0x3BC));
    }
};

struct ItemListEntry {
    int itemDef;
    int paintKit;
    int paintKitSeed;
    float paintKitWear;
    std::uint32_t stickerKit;
    std::uint32_t musicKit;
    bool isNestedList;
    bool isUnusualList;
    PAD(2)

    auto weaponId() const noexcept
    {
        return static_cast<WeaponId>(itemDef);
    }
};

class EconLootListDefinition {
public:
    INCONSTRUCTIBLE(EconLootListDefinition)

    VIRTUAL_METHOD(const char*, getName, 0, (), (this))
    VIRTUAL_METHOD(const UtlVector<ItemListEntry>&, getLootListContents, 1, (), (this))

    bool willProduceStatTrak() noexcept
    {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + WIN32_LINUX(0x38, 0x58));
    }
};

class EconItemSetDefinition {
public:
    INCONSTRUCTIBLE(EconItemSetDefinition)

    VIRTUAL_METHOD(const char*, getLocKey, 1, (), (this))
    VIRTUAL_METHOD(int, getItemCount, 4, (), (this))
    VIRTUAL_METHOD(WeaponId, getItemDef, 5, (int index), (this, index))
    VIRTUAL_METHOD(int, getItemPaintKit, 6, (int index), (this, index))
};

struct EconItemQualityDefinition {
    int value;
    const char* name;
    unsigned weight;
    bool explicitMatchesOnly;
    bool canSupportSet;
    const char* hexColor;
};

struct AlternateIconData {
    UtlString simpleName;
    UtlString largeSimpleName;
    UtlString iconURLSmall;
    UtlString iconURLLarge;
    PAD(WIN32_LINUX(28, 48))
};

struct EconMusicDefinition {
    int id;
    const char* name;
    const char* nameLocalized;
    PAD(2 * sizeof(const char*))
    const char* inventoryImage;
};

class EconItemAttributeDefinition;

class ItemSchema {
public:
    INCONSTRUCTIBLE(ItemSchema)

    PAD(WIN32_LINUX(0x88, 0xB8))
    UtlMap<int, EconItemQualityDefinition> qualities;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, EconItemDefinition*> itemsSorted;
    PAD(WIN32_LINUX(0x60, 0x88))
    UtlMap<int, const char*> revolvingLootLists;
    PAD(WIN32_LINUX(0x80, 0xB0))
    UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, PaintKit*> paintKits;
    UtlMap<int, StickerKit*> stickerKits;
    PAD(WIN32_LINUX(0x11C, 0x1A0))
    UtlMap<int, EconMusicDefinition*> musicKits;

    VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionInterface, 4, (int id), (this, id))
    VIRTUAL_METHOD(const char*, getRarityName, 19, (uint8_t rarity), (this, rarity))
    VIRTUAL_METHOD(EconItemAttributeDefinition*, getAttributeDefinitionInterface, 27, (int index), (this, index))
    VIRTUAL_METHOD(int, getItemSetCount, 28, (), (this))
    VIRTUAL_METHOD(EconItemSetDefinition*, getItemSet, 29, (int index), (this, index))
    VIRTUAL_METHOD(EconLootListDefinition*, getLootList, 31, (const char* name, int* index = nullptr), (this, name, index))
    VIRTUAL_METHOD(EconLootListDefinition*, getLootList, 32, (int index), (this, index))
    VIRTUAL_METHOD(int, getLootListCount, 34, (), (this))
    VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionByName, 42, (const char* name), (this, name))

    auto getItemDefinitionInterface(WeaponId id) noexcept
    {
        return getItemDefinitionInterface(static_cast<int>(id));
    }
};

class ItemSystem {
public:
    INCONSTRUCTIBLE(ItemSystem)

    VIRTUAL_METHOD(ItemSchema*, getItemSchema, 0, (), (this))
};

enum TournamentTeam : std::uint8_t {
    None = 0,
    NinjasInPyjamas = 1,
    AstanaDragons = 2,
    ComplexityGaming = 3,
    VeryGames = 4,
    IBUYPOWER = 5,
    Fnatic = 6,
    ClanMystik = 7,
    RecursiveEsports = 8,
    LGBEsports = 9,
    CopenhagenWolves = 10,
    UniversalSoldiers = 11,
    NatusVincere = 12,
    NFaculty = 13,
    SKGaming = 14,
    Xapso = 15,
    _ReasonGaming = 16,
    TeamDignitas = 24,
    HellRaisers = 25,
    TeamLDLC = 26,
    Titan = 27,
    _3DMax = 28,
    Mousesports = 29,
    ReasonGaming = 30,
    VirtusPro = 31,
    VoxEminor = 32,
    Cloud9 = 33,
    DATTeam = 34,
    EpsilonEsports = 35,
    LondonConspiracy = 36,
    TeamWolf = 37,
    MyXMG = 38,
    PENTASports = 39,
    BravadoGaming = 40,
    PlanetkeyDynamics = 41,
    ESCGaming = 42,
    Flipsid3Tactics = 43,
    TeamEnVyUs = 46,
    VexedGaming = 47,
    TeamLiquid = 48,
    CounterLogicGaming = 49,
    KeydStars = 50,
    TeamSoloMidKinguin = 51,
    Cloud9G2A = 52,
    Renegades = 53,
    TeamImmunity = 54,
    TeamKinguin = 55,
    TeamEBettle = 56,
    LuminosityGaming = 57,
    TeamSoloMid = 58,
    G2Esports = 59,
    Astralis = 60,
    FaZeClan = 61,
    Splyce = 62,
    GambitEsports = 63,
    AllStarTeamAmerica = 64,
    AllStarTeamEurope = 65,
    OpTicGaming = 66,
    GODSENT = 67,
    North = 68,
    BIG = 69,
    VegaSquadron = 70,
    Immortals = 71,
    SproutEsports = 72,
    SpaceSoldiers = 73,
    Tyloo = 74,
    Avangar = 75,
    QuantumBellatorFire = 76,
    MisfitsGaming = 77,
    _100Thieves = 78,
    FlashGaming = 79,
    MIBR = 80,
    TeamSpirit = 81,
    Rogue = 82,
    WinstrikeTeam = 83,
    ENCE = 84,
    FURIA = 85,
    GrayhoundGaming = 86,
    NRG = 87,
    ViCiGaming = 88,
    Vitality = 89,
    Heroic = 95,
    EvilGeniuses = 98,
    CopenhagenFlames = 101,
    paiNGaming = 102,
    MovistarRiders = 103,
    SharksEsports = 104,
    Entropiq = 105,
    MOUZ = 106
};

enum TournamentStage : std::uint8_t {
    GroupStage = 2,
    Quarterfinal = 5,
    Semifinal = 8,
    GrandFinal = 11,
    AllStar = 14,
    ChallengersStage = 27
};

enum ProPlayer {
    f0rest = 93724,
    allu = 1345246,
    friberg = 24295201,
    Xizt = 26224992,
    GeT_RiGhT = 21771190,
    tarik = 18216247,
    reltuC = 518760,
    FNS = 17564706,
    jdm64 = 7223652,
    hazed = 30305781,
    AZR = 24832266,
    SPUNJ = 34303888,
    jks = 16839456,
    Havoc = 10025211,
    yam = 30659,
    device = 27447936,
    dupreeh = 44589228,
    karrigan = 29164525,
    Xyp9x = 30416534,
    cajunb = 18062315,
    Maniac = 35761,
    shox = 46654567,
    RpK = 53985773,
    Ex6TenZ = 11737333,
    SmithZz = 14321919,
    bondik = 46918643,
    DavCost = 20273529,
    WorldEdit = 36732188,
    apEX = 29478439,
    kennyS = 64640068,
    kioShiMa = 40517167,
    Happy = 17975624,
    NBK = 444845,
    fox = 1939536,
    ScreaM = 28502520,
    Maikelele = 925972,
    rain = 37085479,
    boltz = 58113672,
    fer = 38921219,
    coldzera = 79720871,
    FalleN = 424467,
    steel = 54512474,
    nex = 90378773,
    denis = 31185376,
    chrisJ = 28273376,
    gobb = 1162165,
    Spiidi = 13465075,
    markeloff = 5667261,
    B1ad3 = 53258137,
    Hyper = 10357481,
    GruBy = 44752530,
    olofmeister = 28361465,
    pronax = 9419182,
    flusha = 31082355,
    KRIMZ = 71385856,
    JW = 71288472,
    flamie = 156257548,
    seized = 3648428,
    Edward = 23429534,
    GuardiaN = 12065295,
    Zeus = 59062744,
    peet = 104340617,
    rallen = 31166738,
    pashaBiceps = 13580090,
    TaZ = 234052,
    byali = 18860354,
    Snax = 21875845,
    NEO = 460206,
    USTILO = 18903255,
    Rickeh = 3215921,
    emagine = 19633654,
    James = 35336006,
    SnypeR = 7436549,
    seangares = 164004,
    Skadoodle = 21075725,
    shroud = 4515926,
    n0thing = 755286,
    freakazoid = 16883071,
    dennis = 108076825,
    Furlan = 177495873,
    NiKo = 81417650,
    jkaem = 42442914,
    EliGE = 106428011,
    FugLy = 108760082,
    Hiko = 2791,
    AdreN = 46200979,
    nitr0 = 35624002,
    aizy = 90685224,
    Kjaerbye = 59614824,
    Pimp = 57742580,
    tenzki = 37214922,
    MSL = 24134891,
    Shara = 53330928,
    THREAT = 158900,
    fnx = 170178574,
    TACO = 52876568,
    adreN = 1366033,
    s1mple = 73936547,
    DAVEY = 100534297,
    Professor_Chaos = 2029235,
    arya = 411777,
    abE = 25060851,
    jasonR = 24868593,
    DEVIL = 44238623,
    mou = 52678767,
    Dosia = 146641530,
    hooch = 5809933,
    wayLander = 38340970,
    k0nfig = 19403447,
    RUBINO = 35103983,
    gla1ve = 50245293,
    pita = 26459,
    koosta = 161590,
    spaze = 109497526,
    RUSH = 63326592,
    mixwell = 27224124,
    daps = 19892353,
    NAF = 40885967,
    stanislaw = 21583315,
    pyth = 57312567,
    bodyy = 53029647,
    Magisk = 23690923,
    Hobbit = 68027030,
    discodoplan = 161262907,
    twist = 19979131,
    DeadFox = 29338592,
    ANGE1 = 58930363,
    STYKO = 55928431,
    Zero = 34322135,
    loWel = 34456844,
    SIXER = 3429256,
    Lekr0 = 1093135,
    znajder = 97816050,
    electronic = 83779379,
    oskar = 171425088,
    ropz = 31006590,
    mir = 40562076,
    hutji = 61587630,
    jR = 43490511,
    keshandr = 65572922,
    chopper = 85633136,
    felps = 22765766,
    kRYSTAL = 17526007,
    suNny = 57405333,
    zehN = 16308501,
    innocent = 26563533,
    HS = 3417033,
    tabseN = 1225952,
    keev = 51271036,
    LEGIJA = 21242287,
    Stewie2K = 38738282,
    autimatic = 94605121,
    LUCAS1 = 4780624,
    kNgV = 6732863,
    HEN1 = 57761535,
    Nifty = 163358521,
    qikert = 166970562,
    buster = 212936195,
    Jame = 75859856,
    dimasick = 825268,
    KrizzeN = 107672171,
    Twistzz = 55989477,
    devoduvek = 116664993,
    AmaNEk = 108679223,
    ShahZaM = 86003016,
    SicK = 23556959,
    XANTARES = 83853068,
    paz = 68524615,
    ngiN = 17887362,
    Calyx = 92280537,
    MAJ3R = 7167161,
    Boombl4 = 185941338,
    waterfaLLZ = 12720124,
    jmqa = 115223433,
    Kvik = 40982505,
    balblna = 50204800,
    Kaze = 16127541,
    Attacker = 88001036,
    Karsa = 85358333,
    LoveYY = 48886373,
    Summer = 52964519,
    xms = 38509481,
    v4lde = 154664140,
    Golden = 116509497,
    fitch = 33208850,
    cadiaN = 43849788,
    vice = 135979468,
    MICHU = 60359075,
    snatchie = 111436809,
    REZ = 73906687,
    BnTet = 111817512,
    xccurate = 177428807,
    somebody = 85131873,
    DD = 169982617,
    captainMo = 109036162,
    smooya = 211423593,
    tiziaN = 37291208,
    crush = 36981424,
    tonyblack = 15738602,
    sdy = 80311472,
    Dima = 51718767,
    S0tF1k = 174857712,
    COLDYY1 = 34364443,
    niko = 29470855,
    ISSAA = 77546728,
    woxic = 123219778,
    gade = 21355604,
    JUGi = 83626376,
    Snappi = 29157337,
    ANDROID = 1936433,
    yay = 57496765,
    dephh = 26995179,
    draken = 159123007,
    Gratisfaction = 5543683,
    Liazz = 112055988,
    Brollan = 178562747,
    advent = 41786057,
    zhokiNg = 99494192,
    Freeman = 208355715,
    aumaN = 46223698,
    BnTeT = 111817512,
    ZywOo = 153400465,
    ALEX = 44605706,
    dexter = 101535513,
    malta = 181905573,
    erkaSt = 131305548,
    sterling = 100224621,
    DickStacy = 192019632,
    xseveN = 52906775,
    Aleksib = 52977598,
    sergej = 67574097,
    Aerial = 2445180
};

class EconItem {
public:
    INCONSTRUCTIBLE(EconItem)

#ifdef _WIN32
    VIRTUAL_METHOD(void, destructor, 0, (), (this, true))
#else
    VIRTUAL_METHOD(void, destructor, 1, (), (this))
#endif

    PAD(2 * sizeof(std::uintptr_t))

    std::uint64_t itemID;
    std::uint64_t originalID;
    void* customDataOptimizedObject;
    std::uint32_t accountID;
    std::uint32_t inventory;
    WeaponId weaponId;

    std::uint16_t origin : 5;
    std::uint16_t quality : 4;
    std::uint16_t level : 2;
    std::uint16_t rarity : 4;
    std::uint16_t dirtybitInUse : 1;

    std::int16_t itemSet;
    int soUpdateFrame;
    std::uint8_t flags;

    void setAttributeValue(int index, void* value) noexcept
    {
        if (const auto attribute = memory->itemSystem()->getItemSchema()->getAttributeDefinitionInterface(index))
            memory->setDynamicAttributeValue(this, attribute, value);
    }

    void setPaintKit(float paintKit) noexcept { setAttributeValue(6, &paintKit); }
    void setSeed(float seed) noexcept { setAttributeValue(7, &seed); }
    void setWear(float wear) noexcept { setAttributeValue(8, &wear); }
    void setMusicID(int musicID) noexcept { setAttributeValue(166, &musicID); }
    void setStatTrak(int value) noexcept { setAttributeValue(80, &value); }
    void setStatTrakType(int type) noexcept { setAttributeValue(81, &type); }
    void setTournamentID(int id) noexcept { setAttributeValue(137, &id); }
    void setTournamentStage(int stage) noexcept { setAttributeValue(138, &stage); }
    void setTournamentTeam1(int team) noexcept { setAttributeValue(139, &team); }
    void setTournamentTeam2(int team) noexcept { setAttributeValue(140, &team); }
    void setTournamentPlayer(int player) noexcept { setAttributeValue(223, &player); }
    void setSpecialEventID(int id) noexcept { setAttributeValue(267, &id); }
    void setIssueDate(std::uint32_t date) noexcept { setAttributeValue(222, &date); }
    void setDropsAwarded(std::uint32_t n) noexcept { setAttributeValue(237, &n); }
    void setDropsRedeemed(std::uint32_t n) noexcept { setAttributeValue(240, &n); }

    void setStickerID(int slot, int stickerID) noexcept
    {
        if (slot >= 0 && slot <= 5)
            setAttributeValue(113 + 4 * slot, &stickerID);
    }

    void setStickerWear(int slot, float wear) noexcept
    {
        if (slot >= 0 && slot <= 5)
            setAttributeValue(114 + 4 * slot, &wear);
    }
};

class SharedObject {
public:
    INCONSTRUCTIBLE(SharedObject)

    VIRTUAL_METHOD_V(int, getTypeID, 1, (), (this))
};

template <typename T>
class SharedObjectTypeCache {
public:
    INCONSTRUCTIBLE(SharedObjectTypeCache)

    VIRTUAL_METHOD_V(void, addObject, 1, (T* object), (this, object))
    VIRTUAL_METHOD_V(void, removeObject, 3, (T* object), (this, object))

    PAD(sizeof(std::uintptr_t))
    T** objects;
    PAD(WIN32_LINUX(16, 24))
    int objectCount;
    PAD(WIN32_LINUX(4, 12))
    int classID; // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_constants.h#L39
};

template <typename T>
class ClientSharedObjectCache {
public:
    INCONSTRUCTIBLE(ClientSharedObjectCache)

    PAD(16)
    UtlVector<SharedObjectTypeCache<T>*> sharedObjectTypeCaches;

    SharedObjectTypeCache<T>* findBaseTypeCache(int classID) noexcept
    {
        return memory->createBaseTypeCache(this, classID);
    }
};

struct SOID {
    std::uint64_t id;
    std::uint32_t type;
    std::uint32_t padding;
};

class EconItemView;

class CSPlayerInventory {
public:
    INCONSTRUCTIBLE(CSPlayerInventory)

    VIRTUAL_METHOD(void, soCreated, 0, (SOID owner, SharedObject* object, int event), (this, owner, object, event))
    VIRTUAL_METHOD(void, soUpdated, 1, (SOID owner, SharedObject* object, int event), (this, owner, object, event))
    VIRTUAL_METHOD(void, soDestroyed, 2, (SOID owner, SharedObject* object, int event), (this, owner, object, event))
    VIRTUAL_METHOD_V(EconItemView*, getItemInLoadout, 8, (Team team, int slot), (this, team, slot))
    VIRTUAL_METHOD_V(void, removeItem, 15, (std::uint64_t itemID), (this, itemID))

    auto getSOC() noexcept
    {
        return *reinterpret_cast<ClientSharedObjectCache<EconItem>**>(std::uintptr_t(this) + WIN32_LINUX(0x90, 0xC8));
    }

    SharedObjectTypeCache<EconItem>* getItemBaseTypeCache() noexcept
    {
        const auto soc = getSOC();
        if (!soc)
            return nullptr;

        return soc->findBaseTypeCache(1);
    }

    std::pair<std::uint64_t, std::uint32_t> getHighestIDs() noexcept
    {
        const auto soc = getSOC();
        if (!soc)
            return {};

        const auto baseTypeCache = soc->findBaseTypeCache(1);
        if (!baseTypeCache)
            return {};

        std::uint64_t maxItemID = 0;
        std::uint32_t maxInventoryID = 0;
        for (int i = 0; i < baseTypeCache->objectCount; ++i) {
            const auto item = baseTypeCache->objects[i];
            if (const auto isDefaultItem = (item->itemID & 0xF000000000000000) != 0)
                continue;
            maxItemID = (std::max)(maxItemID, item->itemID);
            maxInventoryID = (std::max)(maxInventoryID, item->inventory);
        }

        return std::make_pair(maxItemID, maxInventoryID);
    }

    auto getAccountID() noexcept
    {
        return *reinterpret_cast<std::uint32_t*>(std::uintptr_t(this) + WIN32_LINUX(0x8, 0x10));
    }

    auto getSOID() noexcept
    {
        return *reinterpret_cast<SOID*>(std::uintptr_t(this) + WIN32_LINUX(0x8, 0x10));
    }
};

class InventoryManager {
public:
    INCONSTRUCTIBLE(InventoryManager)

    VIRTUAL_METHOD_V(bool, equipItemInSlot, 20, (Team team, int slot, std::uint64_t itemID, bool swap = false), (this, team, slot, itemID, swap))
    VIRTUAL_METHOD_V(CSPlayerInventory*, getLocalInventory, 23, (), (this))
};
