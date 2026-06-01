#pragma once

#include <string_view>

#include <Config/LanguageConfigVariables.h>
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
        } else if (command == "language") {
            unsigned int langValue = 0;
            const auto langStr = parseNextCommand();
            StringParser{langStr.data()}.parseInt(langValue);
            if (langValue <= 1)
                hookContext.config().template setVariable<Language>(static_cast<lang_vars::Language>(langValue));
        }
    }

    StringParser parser;
    UnloadFlag& unloadFlag;
    HookContext& hookContext;
};
