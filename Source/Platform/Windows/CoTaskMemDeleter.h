#pragma once

#include "DLLs/CombaseDll.h"

struct CoTaskMemDeleter {
    void operator()(void* ptr) const noexcept
    {
        if (const auto coTaskMemFree = CombaseDll{}.CoTaskMemFree())
            coTaskMemFree(ptr);
    }
};
