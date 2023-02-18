#pragma once

#include <string_view>

#include "../JsonForward.h"
#include "../Memory.h"
#include <Config/ResetConfigurator.h>

class Sound {
public:
    Sound(const ClientInterfaces& clientInterfaces, const Memory& memory)
        : clientInterfaces{ clientInterfaces }, memory{ memory }
    {
        ResetConfigurator configurator;
        configure(configurator);
    }

    void modulateSound(std::string_view name, int entityIndex, float& volume) noexcept;

    // GUI
    void menuBarItem() noexcept;
    void tabItem() noexcept;
    void drawGUI(bool contentOnly) noexcept;

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Chicken volume", chickenVolume).def(100);
        configurator("Players", players);
    }

private:
    ClientInterfaces clientInterfaces;
    const Memory& memory;
    bool soundWindowOpen = false;

    int chickenVolume;

    struct Player {
        int masterVolume;
        int headshotVolume;
        int weaponVolume;
        int footstepVolume;

        template <typename Configurator>
        void configure(Configurator& configurator)
        {
            configurator("Master volume", masterVolume).def(100);
            configurator("Headshot volume", headshotVolume).def(100);
            configurator("Weapon volume", weaponVolume).def(100);
            configurator("Footstep volume", footstepVolume).def(100);
        }
    };

    std::array<Player, 3> players;
};
