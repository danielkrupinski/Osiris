#pragma once

#include "StrongType.h"

#include <CS2/Classes/CLoopModeGame.h>
#include <CS2/Classes/GlobalVars.h>
#include <CS2/Classes/ClientModeCSNormal.h>
#include <CS2/Classes/CViewRender.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/Glow.h>
#include <CS2/Classes/VMatrix.h>
#include <CS2/Panorama/CPanel2D.h>
#include <Utils/ReturnAddress.h>

STRONG_TYPE(MainMenuPanelPointer, cs2::CPanel2D**);
STRONG_TYPE(HudPanelPointer, cs2::CPanel2D**);
STRONG_TYPE(LoopModeGamePointer, cs2::CLoopModeGame**);
STRONG_TYPE(GlobalVarsPointer, cs2::GlobalVars**);
STRONG_TYPE(ClientModePointer, cs2::ClientModeCSNormal*);
STRONG_TYPE(GetWorldSessionInClientMode, ReturnAddress);
STRONG_TYPE(TransformTranslate3dVMT, const void*);
STRONG_TYPE(TransformScale3dVMT, const void*);
STRONG_TYPE(WorldToProjectionMatrixPointer, cs2::VMatrix*);
STRONG_TYPE(ViewToProjectionMatrixPointer, cs2::VMatrix*);
STRONG_TYPE(ViewRenderPointer, cs2::CViewRender**);
STRONG_TYPE(LocalPlayerControllerPointer, cs2::CCSPlayerController**);
STRONG_TYPE(ManageGlowSceneObjectPointer, cs2::ManageGlowSceneObject*);
