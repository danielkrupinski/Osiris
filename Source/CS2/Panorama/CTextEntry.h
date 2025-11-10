#pragma once

#include "CPanel2D.h"

namespace cs2
{

struct CTextEntry : CPanel2D {
    using SetText = void(CTextEntry* thisptr, const char* text);

    using TextEntrySubmitEventHandler = bool(void* delegateThisptr, CPanel2D* panel, const char* value);
};

}
