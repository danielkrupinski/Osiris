#pragma once

#include <CS2/Panorama/CUIEngine.h>

#include "StrongType.h"

STRONG_TYPE(UiEnginePointer, cs2::CUIEngine**);
STRONG_TYPE(GetPanelHandleFunctionPointer, cs2::CUIEngine::getPanelHandle);
STRONG_TYPE(GetPanelPointerFunctionPointer, cs2::CUIEngine::getPanelPointer);
STRONG_TYPE(RunScriptFunctionPointer, cs2::CUIEngine::runScript);
STRONG_TYPE(MakeSymbolFunctionPointer, cs2::CUIEngine::makeSymbol);
STRONG_TYPE(OnDeletePanelFunctionPointer, cs2::CUIEngine::onDeletePanel);
