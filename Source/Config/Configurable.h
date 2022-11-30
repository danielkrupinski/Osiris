#pragma once

#include <concepts>

template <typename T, typename Configurator>
concept Configurable = requires(T& a, Configurator& configurator)
{
    { a.configure(configurator) } -> std::same_as<void>; 
};
