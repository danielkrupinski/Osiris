/*
x86RetSpoof - https://github.com/danielkrupinski/x86RetSpoof

Invoke functions with a spoofed return address.
For 32-bit Windows binaries.
Supports __fastcall, __thiscall, __stdcall and __cdecl calling conventions.
Written in C++17.
*/

/*
MIT License

Copyright (c) 2022 Daniel Krupiński

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <cstdint>
#include <utility>

namespace x86RetSpoof
{
    namespace detail
    {
        struct Context;
    }

    template <typename T, typename... Args>
    T invokeFastcall(std::uintptr_t ecx, std::uintptr_t edx, std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, Args... args) noexcept
    {
        detail::Context context;
        return invokeFastcall<T, Args...>(ecx, edx, context, functionAddress, gadgetAddress, args...);
    }

    template <typename T, typename... Args>
    T invokeThiscall(std::uintptr_t ecx, std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, Args... args) noexcept
    {
        return invokeFastcall<T, Args...>(ecx, 0, functionAddress, gadgetAddress, args...);
    }

    template <typename T, typename... Args>
    T invokeStdcall(std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, Args... args) noexcept
    {
        return invokeThiscall<T, Args...>(0, functionAddress, gadgetAddress, args...);
    }

    template <typename T, typename... Args>
    T invokeCdecl(std::uintptr_t functionAddress, std::uintptr_t gadgetAddress, Args... args) noexcept
    {
        detail::Context context;
        return invokeCdecl<T, Args...>(context, functionAddress, gadgetAddress, args...);
    }

    namespace detail
    {
        struct Context {
            std::uintptr_t ebxBackup;
            std::uintptr_t addressToJumpToInGadget;
            std::uintptr_t invokerReturnAddress;
        };

        template <typename T, typename... Args>
        __declspec(naked) T __fastcall invokeFastcall([[maybe_unused]] std::uintptr_t ecx, [[maybe_unused]] std::uintptr_t edx, [[maybe_unused]] Context& context, [[maybe_unused]] std::uintptr_t functionAddress, [[maybe_unused]] std::uintptr_t gadgetAddress, [[maybe_unused]] Args... args) noexcept
        {
            __asm {
                mov eax, [esp + 4] // load a reference (pointer) to context into eax
                mov [eax], ebx // save ebx in context.ebxBackup
                lea ebx, returnHereFromGadget // load the address of the label we want the gadget to jump to
                mov [eax + 4], ebx // save the address of 'returnHereFromGadget' in context.addressToJumpToInGadget
                mov ebx, [esp] // load return address into ebx
                mov [eax + 8], ebx // save return address in context.invokerReturnAddress

                add esp, 12 // skip return address, 'context' and 'functionAddress' on stack, esp will point to the spoofed return address (gadgetAddress)
                lea ebx, [eax + 4] // load the address of context.addressToJumpToInGadget to ebx
                jmp dword ptr[esp - 4] // jump to the function at functionAddress

             returnHereFromGadget:
                push [ebx + 4] // restore context.invokerReturnAddress as a return address
                mov ebx, [ebx - 4] // restore ebx from context.ebxBackup 
                ret
            }
        }

        template <typename T, typename... Args>
        __declspec(naked) T __cdecl invokeCdecl([[maybe_unused]] Context& context, [[maybe_unused]] std::uintptr_t functionAddress, [[maybe_unused]] std::uintptr_t gadgetAddress, [[maybe_unused]] Args... args) noexcept
        {
            __asm {
                mov eax, [esp + 4] // load a reference (pointer) to context into eax
                mov [eax], ebx // save ebx in context.ebxBackup
                lea ebx, returnHereFromGadget // load the address of the label we want the gadget to jump to
                mov [eax + 4], ebx // save the address of 'returnHereFromGadget' in context.addressToJumpToInGadget
                mov ebx, [esp] // load return address into ebx
                mov [eax + 8], ebx // save return address in context.invokerReturnAddress

                add esp, 12 // skip return address, 'context' and 'functionAddress' on stack, esp will point to the spoofed return address (gadgetAddress)
                lea ebx, [eax + 4] // load the address of context.addressToJumpToInGadget to ebx
                jmp dword ptr[esp - 4] // jump to the function at functionAddress

             returnHereFromGadget:
                sub esp, 12
                push [ebx + 4] // restore context.invokerReturnAddress as a return address
                mov ebx, [ebx - 4] // restore ebx from context.ebxBackup 
                ret
            }
        }
    }
}
