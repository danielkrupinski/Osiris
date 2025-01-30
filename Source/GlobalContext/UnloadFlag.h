#pragma once

struct UnloadFlag {
    void set() noexcept
    {
        unload = true;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return unload;
    }

private:
    bool unload{ false };
};
