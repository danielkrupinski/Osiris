#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "SkinChanger.h"

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

                if (weapon->getProperty<WeaponId>("m_iItemDefinitionIndex") == WeaponId::Ak47) {
                    weapon->setProperty<int>("m_iItemIDHigh", -1);
                    weapon->setProperty<int>("m_nFallbackPaintKit", 801);
                    weapon->setProperty<float>("m_flFallbackWear", 0.000000001f);
                }
            }
        }
    }
}
