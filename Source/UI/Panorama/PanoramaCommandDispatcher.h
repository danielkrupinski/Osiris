#pragma once

#include <string_view>

#include <GlobalContext/UnloadFlag.h>
#include <Utils/StringParser.h>

#include "SetCommandHandler.h"

template <typename HookContext>
struct PanoramaCommandDispatcher {
    PanoramaCommandDispatcher(const char* commandline, UnloadFlag& unloadFlag, HookContext& hookContext) noexcept
        : parser{commandline}
        , unloadFlag{unloadFlag}
        , hookContext{hookContext}
    {
    }

    void operator()() noexcept
    {
        std::string_view command{ parseNextCommand() };
        while (!command.empty()) {
            handleCommand(command);
            command = parseNextCommand();
        }
    }

private:
    [[nodiscard]] std::string_view parseNextCommand() noexcept
    {
        return parser.getLine(' ');
    }

    void handleCommand(std::string_view command) noexcept
    {
        if (command == "unload") {
            unloadFlag.set();
        } else if (command == "set") {
            SetCommandHandler{parser, hookContext}();
        } else if (command == "restore_defaults") {
            hookContext.config().restoreDefaults();
        }
    }

    StringParser parser;
    UnloadFlag& unloadFlag;
    HookContext& hookContext;
};
