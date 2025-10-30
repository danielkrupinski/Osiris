#pragma once

#include <gmock/gmock.h>

MATCHER_P(PanelFontParamsEq, panelFontParams, "PanelFontParams")
{
    return testing::ExplainMatchResult(testing::AllOf(
        testing::Field("fontFamily", &PanelFontParams::fontFamily, panelFontParams.fontFamily),
        testing::Field("fontSize", &PanelFontParams::fontSize, panelFontParams.fontSize),
        testing::Field("fontWeight", &PanelFontParams::fontWeight, panelFontParams.fontWeight)
    ), arg, result_listener);
}

MATCHER_P(PanelAlignmentParamsEq, panelAlignmentParams, "PanelAlignmentParams")
{
    return testing::ExplainMatchResult(testing::AllOf(
        testing::Field("horizontalAlignment", &PanelAlignmentParams::horizontalAlignment, panelAlignmentParams.horizontalAlignment),
        testing::Field("verticalAlignment", &PanelAlignmentParams::verticalAlignment, panelAlignmentParams.verticalAlignment)
    ), arg, result_listener);
}

MATCHER_P(PanelMarginParamsEq, panelMarginParams, "PanelMarginParams")
{
    return testing::ExplainMatchResult(testing::AllOf(
        testing::Field("marginLeft", &PanelMarginParams::marginLeft, panelMarginParams.marginLeft),
        testing::Field("marginTop", &PanelMarginParams::marginTop, panelMarginParams.marginTop),
        testing::Field("marginRight", &PanelMarginParams::marginRight, panelMarginParams.marginRight),
        testing::Field("marginBottom", &PanelMarginParams::marginBottom, panelMarginParams.marginBottom)
    ), arg, result_listener);
}
