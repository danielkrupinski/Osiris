#pragma once

template<typename T>
constexpr auto callVirtualFunction(void* classBase, int index) noexcept
{
    int* vmt = *(int**)classBase;
    return reinterpret_cast<T>(vmt[index]);
}
