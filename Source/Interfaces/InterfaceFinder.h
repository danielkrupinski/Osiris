#pragma once

#include <CSGO/Functions.h>
#include <Platform/DynamicLibrary.h>
#include <RetSpoof/FunctionInvoker.h>
#include <RetSpoof/RetSpoofInvoker.h>

struct InterfaceFinder {
    explicit InterfaceFinder(DynamicLibrary library, RetSpoofInvoker invoker)
        : createInterface{ invoker, library.getFunctionAddress("CreateInterface").as<csgo::CreateInterface>() }
    {
    }

    void* operator()(const char* name) const noexcept
    {
        if (createInterface)
            return createInterface(name, nullptr);
        return nullptr;
    }

private:
    FunctionInvoker<csgo::CreateInterface> createInterface;
};
