#pragma once

#include <utility>

#include "KillfeedPreserverContext.h"

template <typename HookContext, typename Context = KillfeedPreserverContext<HookContext>>
class KillfeedPreserver {
public:
    template <typename... Args>
    KillfeedPreserver(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void run() noexcept
    {
        if (context.state().enabled)
            context.deathNotices().forEach(context.preserveDeathNotice());
    }

private:
    Context context;
};
