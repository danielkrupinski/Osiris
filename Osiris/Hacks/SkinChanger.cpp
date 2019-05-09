#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "SkinChanger.h"
#include "../Config.h"

static constexpr int getSkinChangerWeaponIndex(WeaponId weaponId) noexcept
{
    switch (weaponId) {
    default: return -1;

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

    case WeaponId::Bayonet: return 35;
    case WeaponId::Bowie: return 36;
    case WeaponId::Butterfly: return 37;
    case WeaponId::Falchion: return 38;
    case WeaponId::Flip: return 39;
    case WeaponId::Gut: return 40;
    case WeaponId::Huntsman: return 41;
    case WeaponId::Karambit: return 42;
    case WeaponId::M9Bayonet: return 43;
    case WeaponId::Daggers: return 44;
    case WeaponId::Navaja: return 45;
    case WeaponId::Stiletto: return 46;
    case WeaponId::Talon: return 47;
    case WeaponId::Ursus: return 48;
    }
}

void SkinChanger::apply(FrameStage stage) noexcept
{
    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        const auto localPlayerIndex = interfaces.engine->getLocalPlayer();
        const auto localPlayer = interfaces.entityList->getEntity(localPlayerIndex);

        static PlayerInfo playerInfo;
        if (interfaces.engine->getPlayerInfo(localPlayerIndex, playerInfo)) {
            auto weapons = localPlayer->getWeapons();
            for (size_t i = 0; i < 64; i++) {
                Entity* weapon = interfaces.entityList->getEntityFromHandle(weapons[i]);
                if (!weapon)
                    continue;
             //   if (playerInfo.xuidLow != weapon->getProperty<__int32>("m_OriginalOwnerXuidLow")
             //       || playerInfo.xuidHigh != weapon->getProperty<__int32>("m_OriginalOwnerXuidHigh"))
              //      continue;

                auto& currentConfig = config.skinChanger[getSkinChangerWeaponIndex(weapon->getProperty<WeaponId>("m_iItemDefinitionIndex"))];
                if (currentConfig.enabled) {
                    weapon->setProperty<int>("m_iItemIDHigh", -1);
                    weapon->setProperty<int>("m_nFallbackPaintKit", currentConfig.paintKit);
                    weapon->setProperty<float>("m_flFallbackWear", currentConfig.wear);
                }
            }
        }
    }
}
