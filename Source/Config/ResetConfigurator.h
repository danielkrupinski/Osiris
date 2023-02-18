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

    void def(const T& defaultValue)
    {
        variable = defaultValue;
        resetToDefaultConstructed = false;
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

struct ResetConfigurator;

template <typename T, std::size_t N>
struct ResetHandler<std::array<T, N>> {
    explicit ResetHandler(std::array<T, N>& variable)
        : variable{ variable }
    {
    }

    ~ResetHandler() noexcept
    {
        for (auto& element : variable) {
            if constexpr (Configurable<T, ResetConfigurator>) {
                ResetConfigurator configurator;
                element.configure(configurator);
            } else {
                element = T{};
            }
        }
    }

private:
    std::array<T, N>& variable;
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
