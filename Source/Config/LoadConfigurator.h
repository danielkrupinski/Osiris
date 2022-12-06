#pragma once

#include <type_traits>

#include <JsonForward.h>
#include <nlohmann/json.hpp>

#include "Configurable.h"

template <typename T>
constexpr bool jsonValueTypeMatchesType(json::value_t valueType) noexcept
{
    if constexpr (std::is_same_v<T, bool>) {
        return valueType == json::value_t::boolean;
    } else if constexpr (std::is_same_v<T, int>) {
        return valueType == json::value_t::number_integer || valueType == json::value_t::number_unsigned;
    } else if constexpr (std::is_same_v<T, float>) {
        return valueType == json::value_t::number_float;
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type!");
    }
}

template <typename T>
struct LoadHandler {
    LoadHandler(const char* name, T& variable, const json& j)
        : name{ name }, variable{ variable }, j{ j }
    {
    }

    void def([[maybe_unused]] const T& defaultValue) const noexcept
    {
    }

    ~LoadHandler() noexcept
    {
        if (const auto it = j.find(name); it != j.end() && jsonValueTypeMatchesType<T>(it->type()))
            it->get_to(variable);
    }

private:
    const char* name;
    T& variable;
    const json& j;
};

struct LoadConfigurator {
    explicit LoadConfigurator(const json& j)
        : j{ j }
    {
    }

    template <typename T>
    auto operator()(const char* name, T& variable)
    {
        if constexpr (std::is_class_v<T>) {
            static_assert(Configurable<T, LoadConfigurator>, "Class type T must be configurable!");
            if (const auto it = j.find(name); it != j.end() && it->is_object()) {
                LoadConfigurator configurator{ *it };
                variable.configure(configurator);
            }
        } else {
            return LoadHandler<T>{ name, variable, j };
        }
    }

private:
    const json& j;
};
