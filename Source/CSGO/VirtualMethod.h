#pragma once

#include <cstddef>
#include <utility>

#include <Platform/Macros/IsPlatform.h>
#include <RetSpoof/RetSpoofInvoker.h>

template <typename T, typename POD>
class GameClass {
public:
    GameClass(RetSpoofInvoker invoker, POD* pod)
        : invoker{ invoker }, thisptr{ std::uintptr_t(pod) } {}

    template <typename... Args>
    [[nodiscard]] static T from(RetSpoofInvoker invoker, POD* pod, Args&&... args) noexcept
    {
        return T{ GameClass{ invoker, pod }, std::forward<Args>(args)... };
    }

    [[nodiscard]] POD* getPOD() const noexcept
    {
        return reinterpret_cast<POD*>(thisptr);
    }

    template <typename ReturnType, std::size_t Idx, typename... Args>
    constexpr ReturnType call(Args... args) const noexcept
    {
        return invoker.invokeThiscall<ReturnType, Args...>(thisptr, (*reinterpret_cast<std::uintptr_t**>(thisptr))[Idx], args...);
    }

    [[nodiscard]] RetSpoofInvoker getInvoker() const noexcept
    {
        return invoker;
    }

protected:
    [[nodiscard]] std::uintptr_t getThis() const noexcept
    {
        return thisptr;
    }

private:
    RetSpoofInvoker invoker;
    std::uintptr_t thisptr;
};

#define VIRTUAL_METHOD(returnType, name, idx, args, argsRaw) \
returnType name args const noexcept \
{ \
    return call<returnType, idx>argsRaw; \
}

#if IS_WIN32()
#define VIRTUAL_METHOD_V(returnType, name, idx, args, argsRaw) VIRTUAL_METHOD(returnType, name, idx, args, argsRaw)
#else
#define VIRTUAL_METHOD_V(returnType, name, idx, args, argsRaw) VIRTUAL_METHOD(returnType, name, idx + 1, args, argsRaw)
#endif
