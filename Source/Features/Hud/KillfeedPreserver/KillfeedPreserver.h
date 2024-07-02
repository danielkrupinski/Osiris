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
        if (!context.state().enabled)
            return;

        context.deathNotices().forEach([](const auto& deathNotice) {
            if (deathNotice.isLocalPlayerKiller() && deathNotice.wasSpawnedThisRound())
                deathNotice.markAsJustSpawned();
        });
    }

private:
    Context context;
};
