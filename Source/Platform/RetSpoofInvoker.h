#pragma once

#ifdef _WIN32
#include "Windows/RetSpoofInvoker.h"
using RetSpoofInvoker = windows_platform::RetSpoofInvoker;
#endif
