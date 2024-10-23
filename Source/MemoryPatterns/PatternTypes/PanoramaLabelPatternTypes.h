#pragma once

#include <cstdint>

#include <CS2/Panorama/CLabel.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(LabelPanelConstructorPointer, cs2::CLabel::Constructor*);
STRONG_TYPE_ALIAS(SetLabelTextFunctionPointer, cs2::CLabel::setTextInternal);
STRONG_TYPE_ALIAS(LabelPanelObjectSize, std::uint32_t);
