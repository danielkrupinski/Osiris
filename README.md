# Osiris ![](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg) ![](https://img.shields.io/badge/license-MIT-blue.svg)

Free open-source training software / cheat for **Counter-Strike: Global Offensive** game. Designed as internal cheat - injectable DLL. Compatible with the latest version of the game on Steam.

## Features

* **Glow** - render glow effect on entities
    * **Enable** - on / off master switch
    * **Players** - enable glow on players
    * **Enemies only** - glow only opponents
    * **Thickness** - outline thickness
    * **Alpha** - outline alpha
    * **Style** - glow style [0-3]
    * **Allies color**
    * **Enemies color**
    * **Weapons** - enable glow on weapon
    * **Weapons color**
    * **C4** - enable glow on dropped C4 explosive
    * **C4 color**
    * **Planted C4** - enable glow on planted C4 explosive
    * **Planted C4 color**
    * **Chickens** - enable glow on chickens
    * **Chickens color**

* **Triggerbot** - automatically shoot when crosshair is on enemy
    * **Enable** - on / off triggerbot switch, works when you hold **ALT** key
    * **Shot delay** - delay time in ms (milliseconds)

* **Misc**
    * **Bunny hop** - automatically jump to increase movement speed
    * **Clan tag** - set custom clan tag
    * **Disable post-processing** - disable post-processing effects in order to increase FPS
    * **Flash reduction** - reduces flashbang grenade effect [0-255] (0 - full flash, 255 - no flash)
    * **No smoke** (beta) - remove smoke grenade effect
    * **Skybox** - change sky(box)
    * **View model FOV** - change view model FOV [0-130]
    * **World color** - set world ambient color

* **Config**
    * **Load** - load configuration file
    * **Save** - save configuration file
    * **Reset** - restore default configuration settings

## Getting started

### Prerequisites
Microsoft Visual Studio 2017 and Windows SDK 10.0.17134.0 are required in order to compile Osiris. If you don't have ones, you can download VS [here](https://visualstudio.microsoft.com/) (Windows SDK is installed during Visual Studio Setup).

### Cloning
The very first step in order to compile Osiris is to clone this repo from GitHub to your local computer:
```
git clone https://github.com/danielkrupinski/Osiris.git
```

### Compiling from source

When you have equiped a copy of source code, next step is opening **Osiris.sln** in Microsoft Visual Studio 2017.

Then change build configuration to `Release | x86` and simply press **Build solution**.

If everything went right you should receive `Osiris.dll`  binary file.

### Loading / Injecting into game process

Open your favorite [DLL injector](https://en.wikipedia.org/wiki/DLL_injection) and just inject `Osiris.dll` into `csgo.exe` process.

When injected, menu is openable under `INSERT` key.

## FAQ

### How do I open menu?
Press `INSERT` key while in game.

### Where is my config file saved?
Configuration file is saved under name `Osiris.json` in your `Documents` folder (`%USERPROFILE%\Documents`). Of course you can edit config file by hand as it's in JSON format which is easy to read.

## License

> Copyright (c) 2018 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](LICENSE) file for details.
