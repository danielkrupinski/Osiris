#pragma once

#include <array>
#include <type_traits>

#include "Configurable.h"

template <typename T>
struct ResetHandler {
    explicit ResetHandler(T& variable)
        : variable{ variable }
    {
    }

    ResetHandler& def(const T& defaultValue)
    {
        variable = defaultValue;
        resetToDefaultConstructed = false;
        return *this;
    }

    template <typename Functor>
    ResetHandler& loadString(Functor&&)
    {
        return *this;
    }

    template <typename Functor>
    ResetHandler& save(Functor&&)
    {
        return *this;
    }

    ~ResetHandler() noexcept
    {
        if (resetToDefaultConstructed)
            variable = T{};
    }

private:
    T& variable;
    bool resetToDefaultConstructed = true;
};

template <typename T, std::size_t N>
struct ResetHandler<std::array<T, N>> {
    explicit ResetHandler(std::array<T, N>& variable)
        : variable{ variable }
    {
    }

    ResetHandler& def(const std::array<T, N>& defaultValue)
    {
        variable = defaultValue;
        resetToDefaultConstructed = false;
        return *this;
    }

    template <typename Functor>
    ResetHandler& loadString(Functor&&)
    {
        return *this;
    }

    template <typename Functor>
    ResetHandler& save(Functor&&)
    {
        return *this;
    }

    ~ResetHandler() noexcept;

private:
    std::array<T, N>& variable;
    bool resetToDefaultConstructed = true;
};

struct ResetConfigurator {
    template <typename T>
    auto operator()([[maybe_unused]] const char* name, T& variable)
    {
        if constexpr (Configurable<T, ResetConfigurator>) {
            ResetConfigurator configurator;
            variable.configure(configurator);
        } else {
            return ResetHandler<T>{ variable };
        }
    }
};

template <typename T, std::size_t N>
ResetHandler<std::array<T, N>>::~ResetHandler() noexcept
{
    if (!resetToDefaultConstructed)
        return;

    for (auto& element : variable) {
        if constexpr (Configurable<T, ResetConfigurator>) {
            ResetConfigurator configurator;
            element.configure(configurator);
        } else {
            element = T{};
        }
    }
}
