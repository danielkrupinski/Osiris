#pragma once

template <typename Context>
class KillfeedPreserver {
public:
    explicit KillfeedPreserver(Context context) noexcept
        : context{context}
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
