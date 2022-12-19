#pragma once

#include "CdeclFunctionInvoker.h"
#include "FastcallFunctionInvoker.h"
#include "StdcallFunctionInvoker.h"
#include "ThiscallFunctionInvoker.h"

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>

template <typename FunctionType>
struct FunctionInvoker {};

template <typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType (CDECL_CONV*)(Args...)> : CdeclFunctionInvoker<ReturnType, Args...> {
    using CdeclFunctionInvoker<ReturnType, Args...>::CdeclFunctionInvoker;
};

template <typename ReturnType, typename... Args>
FunctionInvoker(RetSpoofInvoker, ReturnType (CDECL_CONV*)(Args...)) -> FunctionInvoker<ReturnType (CDECL_CONV*)(Args...)>;

#if IS_WIN32()

template <typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType (STDCALL_CONV*)(Args...)> : StdcallFunctionInvoker<ReturnType, Args...> {
    using StdcallFunctionInvoker<ReturnType, Args...>::StdcallFunctionInvoker;
};

template <typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType (THISCALL_CONV*)(Args...)> : ThiscallFunctionInvoker<ReturnType, Args...> {
    using ThiscallFunctionInvoker<ReturnType, Args...>::ThiscallFunctionInvoker;
};

template <typename ReturnType, typename... Args>
FunctionInvoker(RetSpoofInvoker, ReturnType (THISCALL_CONV*)(Args...)) -> FunctionInvoker<ReturnType (THISCALL_CONV*)(Args...)>;

template <typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType(FASTCALL_CONV*)(Args...)> : FastcallFunctionInvoker<ReturnType, Args...> {
    using FastcallFunctionInvoker<ReturnType, Args...>::FastcallFunctionInvoker;
};

template <typename ReturnType, typename... Args>
FunctionInvoker(RetSpoofInvoker, ReturnType (FASTCALL_CONV*)(Args...)) -> FunctionInvoker<ReturnType (FASTCALL_CONV*)(Args...)>;

#endif
