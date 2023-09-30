#pragma once

#include "ClientModeImpl.h"
#include "GameRulesImpl.h"
#include "PanoramaImagePanelImpl.h"
#include "PanoramaLabelImpl.h"
#include "PanoramaUiEngineImpl.h"
#include "PanoramaUiPanelImpl.h"
#include "PlantedC4Impl.h"

#include <GlobalContext.h>

inline const ClientModeImpl& ClientModeImpl::instance() noexcept
{
    return GlobalContext::instance().gameClasses->clientMode;
}

inline const GameRulesImpl& GameRulesImpl::instance() noexcept
{
    return GlobalContext::instance().gameClasses->gameRules;
}

inline const PanoramaImagePanelImpl& PanoramaImagePanelImpl::instance() noexcept
{
    return GlobalContext::instance().gameClasses->imagePanel;
}

inline const PanoramaLabelImpl& PanoramaLabelImpl::instance() noexcept
{
    return GlobalContext::instance().gameClasses->panoramaLabel;
}

inline const PanoramaUiEngineImpl& PanoramaUiEngineImpl::instance() noexcept
{
    return GlobalContext::instance().gameClasses->uiEngine;
}

inline const PanoramaUiPanelImpl& PanoramaUiPanelImpl::instance() noexcept
{
    return GlobalContext::instance().gameClasses->panoramaUiPanelOffsets;
}

inline const PlantedC4Impl& PlantedC4Impl::instance() noexcept
{
    return GlobalContext::instance().gameClasses->plantedC4;
}
