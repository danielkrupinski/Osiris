#pragma once

#include "EntitySystemImpl.h"
#include "FileNameSymbolTableImpl.h"
#include "FileSystemImpl.h"
#include "GameRulesImpl.h"
#include "MemAllocImpl.h"
#include "PanelImpl.h"
#include "PanelStyleImpl.h"
#include "PanoramaImagePanelImpl.h"
#include "PanoramaLabelImpl.h"
#include "PanoramaUiEngineImpl.h"
#include "PanoramaUiPanelImpl.h"
#include "PlantedC4Impl.h"
#include "TopLevelWindowImpl.h"

#include <GlobalContext/GlobalContext.h>

inline const EntitySystemImpl& EntitySystemImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().entitySystem;
}

inline const FileNameSymbolTableImpl& FileNameSymbolTableImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().fileNameSymbolTable;
}

inline const FileSystemImpl& FileSystemImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().fileSystem;
}

inline const GameRulesImpl& GameRulesImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().gameRules;
}

inline const MemAllocImpl& MemAllocImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().memAlloc;
}

inline const PanelImpl& PanelImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().panel;
}

inline const PanelStyleImpl& PanelStyleImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().panelStyle;
}

inline const PanoramaImagePanelImpl& PanoramaImagePanelImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().imagePanel;
}

inline const PanoramaLabelImpl& PanoramaLabelImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().panoramaLabel;
}

inline const PanoramaUiEngineImpl& PanoramaUiEngineImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().uiEngine;
}

inline const PanoramaUiPanelImpl& PanoramaUiPanelImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().panoramaUiPanelOffsets;
}

inline const PlantedC4Impl& PlantedC4Impl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().plantedC4;
}

inline const TopLevelWindowImpl& TopLevelWindowImpl::instance() noexcept
{
    return GlobalContext::instance().fullContext().gameClasses().topLevelWindow;
}
