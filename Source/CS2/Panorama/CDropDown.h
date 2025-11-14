#pragma once

#include "CPanel2D.h"
#include "PanelHandle.h"

namespace cs2
{

struct CDropDown : CPanel2D {
    PanelHandle dropdownMenu;

    using SetSelectedIndex = void(CDropDown* thisptr, int index);
    using selectedOption = PanelHandle;

    using DropDownSelectionChangedHandler = bool(void* delegateThisptr, CPanel2D* panel);
};

struct CDropDownMenu : CPanel2D {
    CDropDown* dropdown;
    CPanel2D* selectedChild;
};

}
