#pragma once

#include <array>
#include <type_traits>

#include <JsonForward.h>
#include <nlohmann/json.hpp>

#include "Configurable.h"

template <typename T>
constexpr bool jsonValueTypeMatchesType(json::value_t valueType) noexcept
{
    if constexpr (std::is_same_v<T, bool>) {
        return valueType == json::value_t::boolean;
    } else if constexpr (std::is_same_v<T, int> || std::is_enum_v<T>) {
        return valueType == json::value_t::number_integer || valueType == json::value_t::number_unsigned;
    } else if constexpr (std::is_same_v<T, float>) {
        return valueType == json::value_t::number_float;
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type!");
    }
}

template <typename T>
struct LoadHandler {
    LoadHandler(const json* j, T& variable)
        : j{ j }, variable{ variable }
    {
    }

    LoadHandler& def(const T& /* defaultValue */) noexcept
    {
        return *this;
    }

    template <typename Functor>
    LoadHandler& loadString(Functor&& functor)
    {
        if (j && j->is_string()) {
            functor(j->get<std::string>());
            loaded = true;
        }
        return *this;
    }

    template <typename Functor>
    LoadHandler& save(Functor&&)
    {
        return *this;
    }

    ~LoadHandler() noexcept
    {
        if (!loaded && j && jsonValueTypeMatchesType<T>(j->type()))
            j->get_to(variable);
    }

private:
    T& variable;
    const json* j;
    bool loaded = false;
};

template <typename T, std::size_t N>
struct LoadHandler<std::array<T, N>> {
    LoadHandler(const json* j, std::array<T, N>& variable)
        : j{ j }, variable{ variable }
    {
    }

    LoadHandler& def(const std::array<T, N>& /*defaultValue*/) noexcept
    {
        return *this;
    }

    template <typename Functor>
    LoadHandler& loadString(Functor&& functor)
    {
        if (j && j->is_string()) {
            functor(j->get<std::string>());
            loaded = true;
        }
        return *this;
    }

    template <typename Functor>
    LoadHandler& save(Functor&&)
    {
        return *this;
    }

    ~LoadHandler() noexcept;

private:
    const json* j;
    std::array<T, N>& variable;
    bool loaded = false;
};

struct LoadConfigurator {
    explicit LoadConfigurator(const json& j)
        : j{ j }
    {
    }

    template <typename T>
    auto operator()(const char* name, T& variable)
    {
        if constexpr (Configurable<T, LoadConfigurator>) {
            if (const auto it = j.find(name); it != j.end() && it->is_object()) {
                LoadConfigurator configurator{ *it };
                variable.configure(configurator);
            }
        } else {
            if (const auto it = j.find(name); it != j.end())
                return LoadHandler<T>{ std::to_address(it), variable };
            return LoadHandler<T>{ nullptr, variable };
        }
    }

private:
    const json& j;
};

template <typename T, std::size_t N>
LoadHandler<std::array<T, N>>::~LoadHandler() noexcept
{
    if (!loaded && j && j->is_array() && j->size() == N) {
        std::size_t index = 0;
        for (const auto& element : *j) {
            if constexpr (Configurable<T, LoadConfigurator>) {
                LoadConfigurator configurator{ element };
                variable[index].configure(configurator);
            } else {
                LoadHandler<T>{ &element, variable[index] };
            }
            ++index;
        }
    }
}
