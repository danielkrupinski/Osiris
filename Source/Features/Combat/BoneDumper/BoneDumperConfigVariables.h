#pragma once

#include <Config/ConfigVariable.h>

namespace bone_dumper_vars
{
    // When enabled, every frame the BoneDumper writes a JSON file with the
    // screen-space bounding box of every visible enemy. Pair with a Python
    // screenshot collector running outside the game to build a YOLO dataset
    // automatically (no manual labeling).
    CONFIG_VARIABLE(Enabled, bool, false);
}
