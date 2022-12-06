#pragma once

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

struct ResetConfigurator {
    template <typename T>
    auto operator()([[maybe_unused]] const char* name, T& variable)
    {
        if constexpr (std::is_class_v<T>) {
            static_assert(Configurable<T, ResetConfigurator>, "Class type T must be configurable!");
            ResetConfigurator configurator;
            variable.configure(configurator);
        } else {
            return ResetHandler<T>{ variable };
        }
    }
};
