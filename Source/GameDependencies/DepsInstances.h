#pragma once

#include "FileNameSymbolTableDeps.h"
#include "GameRulesDeps.h"
#include "MemAllocDeps.h"
#include "PanelDeps.h"
#include "PanelStyleDeps.h"
#include "PanoramaImagePanelDeps.h"
#include "PanoramaLabelDeps.h"
#include "PanoramaUiEngineDeps.h"
#include "PanoramaUiPanelDeps.h"
#include "TopLevelWindowDeps.h"

#include <GlobalContext/GlobalContext.h>

inline const FileNameSymbolTableDeps& FileNameSymbolTableDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().fileNameSymbolTableDeps;
}

inline const GameRulesDeps& GameRulesDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().gameRulesDeps;
}

inline const MemAllocDeps& MemAllocDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().memAllocDeps;
}

inline const PanelDeps& PanelDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().panelDeps;
}

inline const PanelStyleDeps& PanelStyleDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().panelStyleDeps;
}

inline const PanoramaImagePanelDeps& PanoramaImagePanelDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().imagePanelDeps;
}

inline const PanoramaLabelDeps& PanoramaLabelDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().panoramaLabelDeps;
}

inline const PanoramaUiEngineDeps& PanoramaUiEngineDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().uiEngineDeps;
}

inline const PanoramaUiPanelDeps& PanoramaUiPanelDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().panoramaUiPanelDeps;
}

inline const TopLevelWindowDeps& TopLevelWindowDeps::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameDependencies().topLevelWindowDeps;
}
