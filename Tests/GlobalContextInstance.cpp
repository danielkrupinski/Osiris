#include <optional>

#include <GlobalContext.h>
#include <Platform/PlatformApi.h>

namespace
{
    std::optional<GlobalContext<PlatformApi>> globalContext;
}

#include "Endpoints.h"

#include "GlobalContext.cpp"
template class GlobalContext<PlatformApi>;
