#pragma once

#include <functional>

#include <d3d9.h>

class Hooks {
private:
    std::function<HRESULT(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)> originalPresent;
};