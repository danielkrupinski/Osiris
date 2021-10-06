#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class GameUI {
public:
    INCONSTRUCTIBLE(GameUI)

    VIRTUAL_METHOD(void, createCommandMsgBox, 19, (const char* title, const char* message, bool showOk = true, bool showCancel = false, const char* okCommand = nullptr, const char* cancelCommand = nullptr, const char* closedCommand = nullptr, const char* legend = nullptr, const char* unknown = nullptr), (this, title, message, showOk, showCancel, okCommand, cancelCommand, closedCommand, legend, unknown))
};
