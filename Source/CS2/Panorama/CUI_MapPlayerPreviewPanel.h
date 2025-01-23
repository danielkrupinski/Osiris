#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/CUtlVector.h>

#include "CPanel2D.h"

namespace cs2
{

struct CUI_MapPlayerPreviewPanel : CPanel2D {
    using EntityHandles = CUtlVector<CEntityHandle>;
};

}
