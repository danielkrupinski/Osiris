#pragma once

#ifdef _WIN32
#ifdef __cplusplus
#include <d3d9.h>
#include <ShlObj.h>
#endif
#include <Windows.h>
#endif

#ifdef __cplusplus
#include <algorithm>
#include <filesystem>
#include <fstream>

#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "nlohmann/json.hpp"
#endif

#ifdef _WIN32
#include "freetype/freetype/internal/ftobjs.h"
#endif
