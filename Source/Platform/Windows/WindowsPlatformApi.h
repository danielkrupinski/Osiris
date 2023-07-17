#pragma once

namespace win { struct Peb; }

struct WindowsPlatformApi {
    [[nodiscard]] static win::Peb* getPeb() noexcept;
};
