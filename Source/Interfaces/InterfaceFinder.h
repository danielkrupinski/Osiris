#pragma once

#include <Platform/DynamicLibraryView.h>
#include <RetSpoof/RetSpoofInvoker.h>

struct InterfaceFinder {
    template <typename DynamicLibraryWrapper>
    explicit InterfaceFinder(DynamicLibraryView<DynamicLibraryWrapper> library, RetSpoofInvoker invoker)
        : createInterfaceFn{ std::uintptr_t(library.getFunctionAddress("CreateInterface")) }, invoker{ invoker }
    {
    }

    void* operator()(const char* name) const noexcept
    {
        if (createInterfaceFn != 0)
            return invoker.invokeCdecl<void*>(createInterfaceFn, name, nullptr);
        return nullptr;
    }

private:
    std::uintptr_t createInterfaceFn;
    RetSpoofInvoker invoker;
};
