#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "SkinChanger.h"
#include "../Config.h"
#include "../nSkinz/SDK/CBaseEntity.hpp"

extern void post_data_update_start(sdk::C_BasePlayer*);

void SkinChanger::run(FrameStage stage) noexcept
{
    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        const auto localPlayerIndex = interfaces.engine->getLocalPlayer();
        const auto localPlayer = interfaces.entityList->getEntity(localPlayerIndex);
        if (localPlayer) post_data_update_start(reinterpret_cast<sdk::C_BasePlayer*>(localPlayer));
    }
}
