#pragma once

#include "FastcallFunctionInvoker.h"
#include <Platform/CallingConventions.h>

template <typename FunctionType>
struct FunctionInvoker {};

template <typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType FASTCALL_CONV (Args...)> : FastcallFunctionInvoker<ReturnType, Args...> {
    using FastcallFunctionInvoker<ReturnType, Args...>::FastcallFunctionInvoker;
};
