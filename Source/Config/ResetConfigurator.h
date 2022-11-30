#pragma once

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
        return ResetHandler<T>{ variable };
    }
};
