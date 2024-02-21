#pragma once

enum class SoundVisualizationPosition {
    AtOrigin,
    AboveOrigin
};

struct SoundVisualizationPanelProperties {
    const char* svgImagePath;
    int svgTextureHeight{-1};
    SoundVisualizationPosition position;
};
