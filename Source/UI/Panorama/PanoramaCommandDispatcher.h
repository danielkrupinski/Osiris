#pragma once

#include <string_view>

#include <Features/Features.h>
#include <Helpers/UnloadFlag.h>
#include <Utils/StringParser.h>

#include "SetCommandHandler.h"

struct PanoramaCommandDispatcher {
    PanoramaCommandDispatcher(const char* commandline, Features features, UnloadFlag& unloadFlag) noexcept
        : parser{commandline}
        , features{features}
        , unloadFlag{unloadFlag}
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
            SetCommandHandler{parser, features}();
        }
    }

    StringParser parser;
    Features features;
    UnloadFlag& unloadFlag;
};
