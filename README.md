# Osiris ![](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg) ![](https://img.shields.io/badge/game-CS%3AGO-yellow.svg) ![](https://img.shields.io/badge/platform-Windows-0078d7.svg) ![](https://img.shields.io/badge/arch-x86-red.svg) ![](https://img.shields.io/badge/license-MIT-blue.svg)

Free open-source training software / cheat for **Counter-Strike: Global Offensive** game. Designed as internal cheat - injectable DLL. Compatible with the latest version of the game on Steam.

## Features
* **Aimbot** - aim assistance
    * **Enable** - on / off master switch
    * **Silent** - aimbot is not visible on your screen (client-sided only)
    * **Fov** - field-of-view which aimbot operates [*0*-*180*]
    * **Smooth** - smooth aimbot movement in order to seem more human-like
    * **Bone** - bone which aimbot aims at
    * **Recoil control x** - horizontal recoil control factor
    * **Recoil control y** - vertical recoil control factor

* **Triggerbot** - automatically fires when crosshair is on enemy
    * **Enable** - on / off triggerbot master switch
    * **On key [ key ]** - triggerbot works only when chosen key is being held
    * **Shot delay** - delay time in ms (milliseconds)

* **Glow** - render glow effect on entities
    * **Enable** - on / off master switch
    * **Thickness** - outline thickness
    * **Alpha** - outline alpha
    * **Style** - glow style [*0*-*3*]
    * **Allies** - glow allies entities
    * **Enemies** - glow enemies entities
    * **Weapons** - glow dropped weapons
    * **C4** - glow dropped C4 explosive
    * **Planted C4** - glow planted bomb
    * **Chickens** - glow chickens entities

* **Chams** - color player models to improve visibility
    * **Enable** - on / off master switch
    * **Alpha** - material transparency
    * **Flat**
    * **Wireframe** - render triangle mesh instead of solid material
    * **Allies** - allies chams on / off and color applied to them
    * **Visible allies** - visible allies chams on / off and color applied to them
    * **Enemies** - enemies chams on / off and color applied to them
    * **Visible enemies** - visible enemies chams on / off and color applied to them
    * **Weapons** - view model weapons chams on / off and color applied to them
    * **Hands** - view model hands chams on / off and color applied to them

* **Visuals** - miscellaneous visual options
    * **Disable post-processing** - disable post-processing effects in order to increase FPS
    * **Inverse ragdoll gravity** - inverse gravitational acceleration on falling player ragdoll corpse (during death sequence)
    * **No sleeves** - removes sleeves model from first-person view
    * **No hands** - removes arms / hands model from first-person view
    * **No weapons** - removes weapons model from first-person view
    * **No smoke** - remove smoke grenade effect
    * **Wireframe smoke** - render smoke skeleton instead of particle effect
   * **Flash reduction** - reduces flashbang grenade effect [*0*-*100*%] (0 - full flash, 100 - no flash)
    * **View model FOV** - change view model FOV [*-60*-*0*-*60*] (0 - actual view model, negative values - decreased view model, positive values - increased view model)
    * **Skybox** - change sky(box)
    * **World color** - set world material ambient light color

* **Misc** - miscellaneous features
    * **Bunny hop** - automatically simulate space bar press / release while jump button is being held; increases movement speed
    * **Clan tag** - set custom clan tag
    * **Animated clan tag** - animate clan tag
    * **Fast duck** - remove crouch delay
    * **Auto pistol** - add ability of firing pistols like automatic rifles
    * **Reveal ranks** - reveal player ranks in competitive modes

* **Config** - configuration system based on cereal binary serialization
    * **Load** - load configuration file from disk
    * **Save** - save configuration file to disk
    * **Reset** - restore default configuration settings (does not touch saved configuration)

## Getting started

### Prerequisites
Microsoft Visual Studio 2017 15.9.4 and Windows SDK 10.0.17763.0 (10.0.17134.0 or lower should work too but requires change in project settings) are required in order to compile Osiris. If you don't have ones, you can download VS [here](https://visualstudio.microsoft.com/) (Windows SDK is installed during Visual Studio Setup).

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
Press `INSERT` key while while focused on CS:GO window.

### Where is my config file saved?
Configuration file is saved under name `Osiris` in your `Documents` folder (`%USERPROFILE%\Documents`). The config is binary serialized so it is not meant to be edited. Sometimes after updates configuration file needs to be deleted and recreated.

### What is cheat execution entry point and where is it located?
Osiris is based on six main global objects ("modules") - see [Osiris.cpp](https://github.com/danielkrupinski/Osiris/blob/master/Osiris/Osiris.cpp) file for detailes. The modules are initialized in the order they are declared in above file as they depend on each other.

## Acknowledgments

* [ocornut](https://github.com/ocornut) and [contributors](https://github.com/ocornut/imgui/graphs/contributors) for creating such amazing GUI library - [imgui](https://github.com/ocornut/imgui).
* [Zer0Mem0ry](https://github.com/Zer0Mem0ry) - for great tutorials on reverse engineering and game hacking

## License

> Copyright (c) 2018-2019 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](LICENSE) file for details.
