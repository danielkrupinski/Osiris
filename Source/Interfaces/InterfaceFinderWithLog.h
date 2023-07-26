#pragma once

#include <string>

#include <Platform/SimpleMessageBox.h>

#include "InterfaceFinder.h"

struct InterfaceFinderWithLog {
    explicit InterfaceFinderWithLog(InterfaceFinder finder)
        : finder{ finder }
    {
    }

    void* operator()(const char* name) const noexcept
    {
        if (const auto foundInterface = finder(name))
            return foundInterface;

        SimpleMessageBox{}.showError("Osiris", ("Failed to find " + std::string{ name } + " interface!").c_str());
        std::exit(EXIT_FAILURE);
    }

private:
    InterfaceFinder finder;
};
