#pragma once

#include "CPanel2D.h"

namespace cs2
{

struct CSlider : CPanel2D {
    using SetValue = void(CSlider* thisptr, float value);

    using SliderValueChangedEventHandler = bool(void* delegateThisptr, CPanel2D* panel, float value);
};

}
