# Osiris [![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg)](https://en.wikipedia.org/wiki/C%2B%2B) [![CS:GO](https://img.shields.io/badge/game-CS%3AGO-yellow.svg)](https://store.steampowered.com/app/730/CounterStrike_Global_Offensive/) [![Windows](https://img.shields.io/badge/platform-Windows-0078d7.svg)](https://en.wikipedia.org/wiki/Microsoft_Windows) [![x86](https://img.shields.io/badge/arch-x86-red.svg)](https://en.wikipedia.org/wiki/X86) [![License](https://img.shields.io/github/license/danielkrupinski/Osiris.svg)](LICENSE) ![](https://img.shields.io/github/issues/danielkrupinski/Osiris.svg)

Free open-source training software / cheat for **Counter-Strike: Global Offensive** game. Designed as internal cheat - [Dynamic-link library](https://en.wikipedia.org/wiki/Dynamic-link_library) (DLL) loadable into game process. Compatible with the latest version of the game on Steam.

## Features
* **Aimbot** - aim assistance
    * **Enabled** - on / off master switch
    * **Silent** - aimbot is not visible on your screen (client-sided only)
    * **Friendly fire** - treat allies as enemies
    * **Visible only** - aim only on visible players
    * **Scoped only** - aimbot works only when using scope (applies only to sniper rifles)
    * **Ignore flash** - ignore flashbang i.e. aim when local player is flashed
    * **Ignore smoke** - ignore smoke i.e. aim when target is in smoke
    * **Auto shot** - shoot automatically when target found
    * **Recoil-based fov** - aimbot uses recoil as fov origin
    * **Fov** - field-of-view which aimbot operates [*0*-*255*]
    * **Max angle delta** - maximum viewangles change per tick
    * **Smooth** - smooth aimbot movement in order to seem more human-like
    * **Bone** - bone which aimbot aims at
    * **Recoil control x** - horizontal recoil control factor
    * **Recoil control y** - vertical recoil control factor

* **Triggerbot** - automatically fires when crosshair is on enemy
    * **Enabled** - on / off master switch
    * **On key [ key ]** - triggerbot works only when chosen key is being held
    * **Friendly fire** - treat allies as enemies
    * **Scoped only** - triggerbot works only when using scope (applies only to sniper rifles)
    * **Ignore flash** - ignore flashbang i.e. shoot when local player is flashed
    * **Ignore smoke** - ignore smoke i.e. shoot when target is in smoke
    * **Hitgroup** - body parts on which triggerbot works
    * **Shot delay** - delay time in ms (milliseconds)
    
* **Backtrack** - abuse lag compensation in order to move players back in time
    * **Enabled** - on / off master switch
    * **Ignore smoke** - ignore smoke i.e. backtrack when target is in smoke
    * **Time limit** - limit the backtracking window [*1*-*200*ms]

* **Glow** - render glow effect on entities
    * **Enabled** - on / off master switch
    * **Health based** - color is based on player's hp
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
    * **Enabled** - on / off master switch
    * **Health based** - color is based on player's hp
    * **Alpha** - material transparency
    * **Material** - material applied to model
    * **Wireframe** - render triangle mesh instead of solid material
    * **Allies** - allies chams on / off and color applied to them
    * **Visible allies** - visible allies chams on / off and color applied to them
    * **Enemies** - enemies chams on / off and color applied to them
    * **Visible enemies** - visible enemies chams on / off and color applied to them
    * **Weapons** - view model weapons chams on / off and color applied to them
    * **Hands** - view model hands chams on / off and color applied to them

* **Esp** - show information about player position
    * **Enabled** - on / off master switch
    * **Snaplines** - draw snaplines to players
    * **Box** - draw 2D box over player model
    * **Name** - draw player name
    * **Head dot** - draw dot on player's head

* **Visuals** - miscellaneous visual options
    * **Disable post-processing** - disable post-processing effects in order to increase FPS
    * **Inverse ragdoll gravity** - inverse gravitational acceleration on falling player ragdoll corpse (during death sequence)
    * **No fog** - remove fog from map for better visibility
    * **No 3d sky** - remove 3d skybox from map - increases FPS
    * **No visual recoil** - remove visual recoil punch effect
    * **No hands** - remove arms / hands model from first-person view
    * **No sleeves** - remove sleeves model from first-person view
    * **No weapons** - remove weapons model from first-person view
    * **No smoke** - remove smoke grenade effect
    * **No blur** - remove blur
    * **No scope overlay** - remove black overlay while scoping
    * **Wireframe smoke** - render smoke skeleton instead of particle effect
   * **Flash reduction** - reduces flashbang grenade effect [*0*-*100*%] (0 - full flash, 100 - no flash)
    * **View model FOV** - change view model FOV [*-60*-*0*-*60*] (0 - actual view model, negative values - decreased view model, positive values - increased view model)
    * **Brightness** - control game brightness [*0.0*-*1.0*]
    * **Skybox** - change sky(box)
    * **World color** - set world material ambient light color

* **Knife changer** - change knife model in first-person view
    * **Enabled** - enable / disable knife changer
    * **Knife** - desired knife model

* **Misc** - miscellaneous features
    * **Auto strafe** - automatically strafe in air following mouse movement
    * **Bunny hop** - automatically simulate space bar press / release while jump button is being held; increases movement speed
    * **Clan tag** - set custom clan tag
    * **Animated clan tag** - animate clan tag
    * **Fast duck** - remove crouch delay
    * **Sniper crosshair** - draw crosshair while holding sniper rifle
    * **Recoil crosshair** - crosshair follows recoil pattern
    * **Auto pistol** - fire pistols like automatic rifles
    * **Auto reload** - automatically reload if weapon has empty clip
    * **Auto accept** - automatically accept competitive match
    * **Radar hack** - show enemies positions on radar
    * **Reveal ranks** - show player ranks in scoreboard in competitive modes
    * **Spectator list** - show nicknames of players spectating you
    * **Watermark** - show cheat name in upper-left screen corner

* **Config** - configuration system based on binary serialization
    * **Create config** - create new configuration file
    * **Reset config** - restore default configuration settings (does not touch saved configuration)
    * **Load selected** - load selected configuration file
    * **Save selected** - save selected configuration file
    * **Delete selected** - delete selected configuration file

## Getting started

### Prerequisites
Microsoft Visual Studio 2019 (preferably latest version i.e. 16.0.3), platform toolset v142 and Windows SDK 10.0 are required in order to compile Osiris. If you don't have ones, you can download VS [here](https://visualstudio.microsoft.com/) (Windows SDK is installed during Visual Studio Setup).

### Cloning
The very first step in order to compile Osiris is to clone this repo from GitHub to your local computer. Git is required to step futher, if not installed download it [here](https://git-scm.com). Open git bash / git cmd / cmd and enter following command:
```
git clone https://github.com/danielkrupinski/Osiris.git
```
`Osiris` folder should have been succesfully created, containing all the source files.

### Compiling from source

When you have equiped a copy of source code, next step is opening **Osiris.sln** in Microsoft Visual Studio 2017.

Then change build configuration to `Release | x86` and simply press **Build solution**.

If everything went right you should receive `Osiris.dll`  binary file.

### Loading / Injecting into game process

Open your favorite [DLL injector](https://en.wikipedia.org/wiki/DLL_injection) and just inject `Osiris.dll` into `csgo.exe` process.

When injected, menu is openable under `INSERT` key.


### Further optimizations
If your CPU supports AVX / AVX2 instruction set, you can enable it in project settings. This should result in more performant code, optimized for your CPU. Currently SSE2 instructions are selected in project settings.

## FAQ

### How do I open menu?
Press `INSERT` key while focused on CS:GO window.

### Where is my config file saved?
Configuration files are saved inside `Osiris` folder in your `Documents` folder (`%USERPROFILE%\Documents\Osiris`). The config is binary serialized so it is not meant to be edited by humans. Sometimes after updates configuration file needs to be deleted and recreated.

### What is cheat execution entry point and where is it located?
Osiris is based on six main global objects ("modules") - see [Osiris.cpp](https://github.com/danielkrupinski/Osiris/blob/master/Osiris/Osiris.cpp) file for detailes. The modules are initialized in the order they are declared in above file as they depend on each other.

## Acknowledgments

* [ocornut](https://github.com/ocornut) and [contributors](https://github.com/ocornut/imgui/graphs/contributors) for creating such amazing GUI library - [imgui](https://github.com/ocornut/imgui).
* [Zer0Mem0ry](https://github.com/Zer0Mem0ry) - for great tutorials on reverse engineering and game hacking

## License

> Copyright (c) 2018-2019 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](LICENSE) file for details.
