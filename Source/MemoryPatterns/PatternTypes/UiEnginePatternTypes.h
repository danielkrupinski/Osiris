#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(UiEnginePointer, cs2::CUIEngine**);
STRONG_TYPE_ALIAS(GetPanelPointerFunctionPointer, cs2::CUIEngine::getPanelPointer);
STRONG_TYPE_ALIAS(RunScriptFunctionPointer, cs2::CUIEngine::runScript);
STRONG_TYPE_ALIAS(MakeSymbolFunctionPointer, cs2::CUIEngine::makeSymbol);
STRONG_TYPE_ALIAS(OnDeletePanelFunctionPointer, cs2::CUIEngine::onDeletePanel);
STRONG_TYPE_ALIAS(RegisterEventHandlerFunctionPointer, cs2::CUIEngine::RegisterEventHandler);
