#pragma once

#include <algorithm>
#include <tuple>
#include <utility>

#include <Platform/Macros/CallingConventions.h>
#include "RetSpoofInvoker.h"

template <typename ReturnType, typename... Args>
class ThiscallFunctionInvoker {
public:
    explicit ThiscallFunctionInvoker(RetSpoofInvoker invoker, std::uintptr_t functionAddress)
        : invoker{ invoker }, functionAddress{ functionAddress }
    {
    }

    explicit ThiscallFunctionInvoker(RetSpoofInvoker invoker, ReturnType (THISCALL_CONV* function)(Args...))
        : invoker{ invoker }, functionAddress{ std::uintptr_t(function) }
    {
    }

    ReturnType operator()(Args... args) const noexcept
    {
        return invoke(std::forward_as_tuple(args...));
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return functionAddress != 0;
    }

private:
    template <typename Tuple>
    ReturnType invoke(const Tuple& args) const noexcept
    {
        constexpr auto numberOfArgs = std::tuple_size_v<Tuple>;
        constexpr auto numberOfRegisterArgs = std::min<std::size_t>(numberOfArgs, 1);
        constexpr auto numberOfStackArgs = numberOfArgs - numberOfRegisterArgs;

        return [this] <std::size_t... I> (const Tuple& tuple, std::index_sequence<I...>) {
            const auto ecx = getRegisterArgument<numberOfRegisterArgs, 0>(tuple);

            return invoker.invokeThiscall<ReturnType>(ecx, functionAddress, std::get<I + numberOfRegisterArgs>(tuple)...);
        }(args, std::make_index_sequence<numberOfStackArgs>{});
    }

    template <std::size_t NumberOfRegisterArgs, std::size_t N, typename Tuple>
    [[nodiscard]] static auto getRegisterArgument(const Tuple& tuple) noexcept
    {
        if constexpr (N < NumberOfRegisterArgs) {
            return std::uintptr_t(std::get<N>(tuple));
        } else {
            return std::uintptr_t{};
        }
    }

    RetSpoofInvoker invoker;
    std::uintptr_t functionAddress;
};
