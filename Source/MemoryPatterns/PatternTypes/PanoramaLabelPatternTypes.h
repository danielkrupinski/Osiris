#pragma once

#include <cstdint>

#include <CS2/Panorama/CLabel.h>
#include "StrongType.h"

STRONG_TYPE(LabelPanelConstructorPointer, cs2::CLabel::Constructor*);
STRONG_TYPE(SetLabelTextFunctionPointer, cs2::CLabel::setTextInternal);
STRONG_TYPE(LabelPanelObjectSize, std::uint32_t);
