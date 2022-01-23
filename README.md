# Osiris 
[![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=plastic)](https://en.wikipedia.org/wiki/C%2B%2B) 
[![CS:GO](https://img.shields.io/badge/game-CS%3AGO-yellow.svg?style=plastic)](https://store.steampowered.com/app/730/CounterStrike_Global_Offensive/) 
[![Windows](https://img.shields.io/badge/platform-Windows-0078d7.svg?style=plastic)](https://en.wikipedia.org/wiki/Microsoft_Windows) 
[![x86](https://img.shields.io/badge/arch-x86-red.svg?style=plastic)](https://en.wikipedia.org/wiki/X86) 
[![License](https://img.shields.io/github/license/danielkrupinski/Osiris.svg?style=plastic)](LICENSE)
[![Issues](https://img.shields.io/github/issues/danielkrupinski/Osiris.svg?style=plastic)](https://github.com/danielkrupinski/Osiris/issues)
[![PayPal](https://img.shields.io/badge/donate-PayPal-104098.svg?style=plastic&logo=PayPal)](https://paypal.me/DanielK19)
<br>![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)
![Linux](https://github.com/danielkrupinski/Osiris/workflows/Linux/badge.svg?branch=master&event=push)

Free open-source cross-platform cheat software for **Counter-Strike: Global Offensive** game. Designed as an internal cheat - [Dynamic-link library](https://en.wikipedia.org/wiki/Dynamic-link_library) (DLL) loadable into game process. Compatible with the Steam version of the game. Available for Windows and Linux systems.

## Features
*   **Aimbot** - aim assistance
*   **Triggerbot** - automatically fires when crosshair is on enemy
*   **Backtrack** - abuse lag compensation to move players back in time
*   **Glow** - render glow effect on entities
*   **Chams** - color player models to improve visibility
*   **Stream Proof ESP** - show information about players, dropped weapons and projectiles
*   **Visuals** - miscellaneous visual options
*   **Inventory Changer** - add weapon skins, stickers, knives, gloves, medals and more to your in-game inventory
*   **Sound** - modify volume of certain sound effects
*   **Style** - select menu window layout and colors
*   **Misc** - miscellaneous features
*   **Config** - JSON-based configuration system

<details>

*   **Aimbot** - aim assistance
    *   **Enabled** - on / off master switch
    *   **On key \[ key \]** - aimbot works only when chosen key is being held
    *   **Aimlock** - brings your aim to the target (affected by Smooth).
    *   **Silent** - aimbot is not visible on your screen (client-sided only)
    *   **Friendly fire** - treat allies as enemies
    *   **Visible only** - aim only on visible players
    *   **Scoped only** - aimbot works only when using scope (applies only to sniper rifles)
    *   **Ignore flash** - ignore flashbang i.e. aim when local player is flashed
    *   **Ignore smoke** - ignore smoke i.e. aim when target is in smoke
    *   **Auto shot** - shoot automatically when target found
    *   **Auto scope** - automatically scopes sniper rifle before shooting
    *   **Bone** - bone which aimbot aims at
    *   **Fov** - field-of-view which aimbot operates \[*0*-*255*\]
    *   **Smooth** - smooth aimbot movement in order to seem more human-like
    *   **Max aim inaccuracy** - maximum weapon inaccuracy allowing aimbot to run, lowering this value will e.g. disable aimbot while jumping or running

*   **Triggerbot** - automatically fires when crosshair is on enemy
    *   **Enabled** - on / off master switch
    *   **On key \[ key \]** - triggerbot works only when chosen key is being held
    *   **Friendly fire** - treat allies as enemies
    *   **Scoped only** - triggerbot works only when using scope (applies only to sniper rifles)
    *   **Ignore flash** - ignore flashbang i.e. shoot when local player is flashed
    *   **Ignore smoke** - ignore smoke i.e. shoot when target is in smoke
    *   **Hitgroup** - body parts on which triggerbot works
    *   **Shot delay** - delay time in ms (milliseconds)
    *   **Min damage** - minimal damage to fire.

*   **Backtrack** - abuse lag compensation in order to move players back in time
    *   **Enabled** - on / off master switch
    *   **Ignore smoke** - ignore smoke i.e. backtrack when target is in smoke
    *   **Time limit** - limit the backtracking window \[*1*-*200*ms\]

*   **Glow** - render glow effect on entities

    *Allies, Enemies, Planting (player planting bomb), Defusing (player defusing bomb), Local player, Weapons (dropped weapons), C4, Planted C4, Chickens, Defuse kits, Projectiles, Hostages, Ragdolls* **/** *All, Visible, Occluded*

    *   **Enabled** - on / off master switch
    *   **Health based** - color is based on player's hp
    *   **Color** - glow color in rgba format
    *   **Style** - glow style { `Default`, `Rim3d`, `Edge`, `Edge Pulse` }

*   **Chams** - color player models to improve visibility

    *Allies, Enemies, Planting (player planting bomb), Defusing (player defusing bomb), Local player, Weapons (dropped weapons), Hands (view model hands), Backtrack (requires backtrack to be enabled), Sleeves (view model)* **/** *All, Visible, Occluded*
    *   **Enabled** - on / off master switch
    *   **Health based** - color is based on player's hp
    *   **Blinking** - change transparency frequently
    *   **Material** - material applied to model { `Normal`, `Flat`, `Animated`, `Platinum`, `Glass`, `Chrome`, `Crystal`, `Silver`, `Gold`, `Plastic`, `Glow` }
    *   **Wireframe** - render triangle mesh instead of solid material
    *   **Cover** - draw chams material on top of the original material instead of overriding it
    *   **Ignore-Z** - draw material through walls

*   **ESP** - show additional information about players and game world
    1.  *Allies, Enemies*
        *   *All, Visible, Occluded*

    2.  *Weapons*

    3.  *Projectiles*
        *   *Flashbang, HE Grenade, Breach Charge, Bump Mine, Decoy Grenade, Molotov, TA Grenade, Smoke Grenade, Snowball*

    4.  *Danger Zone*
        *   *Sentries, Drones, Cash, Cash Dufflebag, Pistol Case, Light Case, Heavy Case, Explosive Case, Tools Case, Full Armor, Armor, Helmet, Parachute, Briefcase, Tablet Upgrade, ExoJump, Ammobox, Radar Jammer*

    *   **Enabled** - on / off master switch
    *   **Font** - esp text font
    *   **Snaplines** - draw snapline to player
    *   **Eye traces** - draw player eye traces (shows where player looks)
    *   **Box** - draw 2D box over player model
    *   **Name** - draw player name
    *   **Health** - draw player health
    *   **Health bar** - draw rectangle indicating player health
    *   **Armor** - draw player armor
    *   **Armor bar** - draw rectangle indicating player armor
    *   **Money** - draw player money
    *   **Head dot** - draw dot on player's head
    *   **Active Weapon** - draw player equipped weapon

*   **Visuals** - miscellaneous visual options
    *   **Disable post-processing** - disable post-processing effects in order to increase FPS
    *   **Inverse ragdoll gravity** - inverse gravitational acceleration on falling player ragdoll corpse (during death sequence)
    *   **No fog** - remove fog from map for better visibility
    *   **No 3d sky** - remove 3d skybox from map - increases FPS
    *   **No visual recoil** - remove visual recoil punch effect
    *   **No hands** - remove arms / hands model from first-person view
    *   **No sleeves** - remove sleeves model from first-person view
    *   **No weapons** - remove weapons model from first-person view
    *   **No smoke** - remove smoke grenade effect
    *   **No blur** - remove blur
    *   **No scope overlay** - remove black overlay while scoping
    *   **No grass** - remove grass from map in Danger Zone mode (`dz_blacksite` and `dz_sirocco` maps)
    *   **No shadows** - disable dynamic shadows
    *   **Wireframe smoke** - render smoke skeleton instead of particle effect
    *   **Zoom \[ key \]** - enable zoom on unzoomable weapons
    *   **Thirdperson** - thirdperson view
    *   **Thirdperson distance** - camera distance in thirdperson view
    *   **View model FOV** - change view model FOV \[*-60*-*0*-*60*\] (0 - actual view model, negative values - decreased view model, positive values - increased view model)
    *   **FOV** - change view FOV \[*-60*-*0*-*60*\] (0 - actual view fov, negative values - decreased, positive values - increased)
    *   **Far Z** - far clipping range, useful after disabling fog on large maps (e.g `dz_sirocco`) to render distant buildings
    *   **Flash reduction** - reduces flashbang grenade effect \[*0*-*100*%\] (0 - full flash, 100 - no flash)
    *   **Brightness** - control game brightness \[*0.0*-*1.0*\]
    *   **Skybox** - change sky(box)
    *   **World color** - set world material ambient light color
    *   **Deagle spinner** - play "spinning" inspect animation when holding Deagle
    *   **Screen effect** - screenspace effect - *Drone cam, Drone cam with noise, Underwater, Healthboost, Dangerzone*
    *   **Hit effect** - show screen effect on enemy hit
    *   **Hit marker** - show a cross detail on enemy hit

*   **Inventory Changer** - add weapon skins, stickers, knives, gloves, medals and more to your in-game inventory

*   **Sound** - modify volume of certain sound effects
    *   **Chicken volume** - volume of chicken sounds

    *Local player, Allies, Enemies*
    *   **Master volume** - overall volume of sounds emitted by player
    *   **Headshot volume** - volume of headshot sound (when player gets headshoted)
    *   **Weapon volume** - volume of player weapon shots
    *   **Footstep volume** - volume of player footsteps

*   **Misc** - miscellaneous features
    *   **Menu key \[ key \]** - menu toggle key

    *   **Menu style** - menu style toggle (*Classic* **/** *One window*)

    *   **Menu colors** - menu color theme (*Dark **/** Light **/** Classic*)

    *   **Anti AFK kick** - avoid auto-kick by server for inactivity

    *   **Auto strafe** - automatically strafe in air following mouse movement

    *   **Bunny hop** - automatically simulate space bar press / release while jump button is being held; increases movement speed

    *   **Clan tag** - set custom clan tag

    *   **Animated clan tag** - animate clan tag

    *   **Fast duck** - remove crouch delay

    *   **Sniper crosshair** - draw crosshair while holding sniper rifle

    *   **Recoil crosshair** - crosshair follows recoil pattern

    *   **Auto pistol** - fire pistols like automatic rifles

    *   **Auto reload** - automatically reload if weapon has empty clip

    *   **Auto accept** - automatically accept competitive match

    *   **Radar hack** - show enemies positions on radar

    *   **Reveal ranks** - show player ranks in scoreboard in competitive modes

    *   **Reveal money** - show enemies' money in scoreboard

    *   **Spectator list** - show nicknames of players spectating you

    *   **Watermark** - show cheat name in the upper-left screen corner and fps & ping in the upper-right corner

    *   **Offscreen Enemies** - draw circles on the screen indicating that there are enemies behind us

    *   **Fix animation LOD** - fix aimbot inaccuracy for players behind local player

    *   **Fix bone matrix** - correct client bone matrix to be closer to server one

    *   **Disable model occlusion** - draw player models even if they are behind thick walls

    *   **Kill message** - print message to chat after killing an enemy

    *   **Name stealer** - mimic other players names

    *   **Custom clantag** - set a custom clantag

    *   **Fast plant** - plants bomb on bombsite border, when holding <kbd>LMB</kbd> or <kbd>E</kbd> key

    *   **Fast Stop** - stops the player faster than normal

    *   **Quick reload** - perform quick weapon switch during reload for faster reload

    *   **Prepare revolver \[ key \]** - keep revolver cocked, optionally on key

    *   **Fix tablet signal** - allow use tablet on underground (dangerzone)

    *   **Hit Sound** - sound emitted when hurting enemy

    *   **Chocked packets** - length of sequence of chocked ticks

    *   **Max angle delta** - maximum viewangles change per tick

    *   **Aspect Ratio** - allows you to change the aspect ratio

    *   **Purchase List** - show the purchased equipment by enemies.

    *   **Reportbot** - automatically report players on server for cheating or other abusive actions
        *   **Enabled** - on / off master switch
        *   **Target** - report target *Enemies/Allies/All*
        *   **Delay** - delay between reports, in seconds
        *   **Aimbot** - report for aim assistance
        *   **Wallhack** - report for visual assistance
        *   **Other** - report for other assistance
        *   **Griefing** - report for griefing
        *   **Abusive Communications** - report for abusive communications

    *   **Unhook** - unload cheat

*   **Config** - JSON-based configuration system
    *   **Create config** - create new configuration file
    *   **Reset config** - restore default configuration settings (does not touch saved configuration)
    *   **Load selected** - load selected configuration file
    *   **Save selected** - save selected configuration file
    *   **Delete selected** - delete selected configuration file
    *   **Reload configs** - reload configs list
</details>

## Getting started

### Prerequisites
Microsoft Visual Studio 2019 16.10 (or newer), platform toolset v142 and Windows SDK 10.0.x.x are required in order to compile Osiris. If you don't have ones, you can download VS [here](https://visualstudio.microsoft.com/) (Windows SDK is installed during Visual Studio Setup).

### Downloading

There are two options of downloading the source code:

#### Without [git](https://git-scm.com)

Choose this option if you want pure source and you're not going to contribute to the repo. Download size ~600 kB.

To download source code this way [click here](https://github.com/danielkrupinski/Osiris/archive/master.zip).

#### With [git](https://git-scm.com)

Choose this option if you're going to contribute to the repo or you want to use version control system. Download size ~4 MB. Git is required to step further, if not installed download it [here](https://git-scm.com).

Open git command prompt and enter following command:

    git clone --depth=1 https://github.com/danielkrupinski/Osiris.git

`Osiris` folder should have been successfully created, containing all the source files.

### Compiling from source

When you have equipped a copy of the source code, next step is opening **Osiris.sln** in Microsoft Visual Studio 2019.

Then change build configuration to `Release | x86` and simply press **Build solution**.

If everything went right you should receive `Osiris.dll`  binary file.

### Loading / Injecting into game process

Open your favorite [DLL injector](https://en.wikipedia.org/wiki/DLL_injection) and just inject `Osiris.dll` into `csgo.exe` process.

When injected, menu is openable under `INSERT` key.

### Further optimizations
If your CPU supports AVX / AVX2 / AVX-512 instruction set, you can enable it in project settings. This should result in more performant code, optimized for your CPU. Currently SSE2 instructions are selected in project settings.

## FAQ

### How do I open menu?
Press <kbd>INSERT</kbd> while focused on CS:GO window.

### Where is my config file saved?
Configuration files are saved inside `Osiris` folder in your `Documents` folder (`%USERPROFILE%\Documents\Osiris`). The config is in human readable format and can be edited (e.g, using notepad). Sometimes after updates configuration file needs to be deleted and recreated.

### What hooking methods Osiris uses?
Currently implemented hooking methods are:
*   MinHook - trampoline hook
*   VmtHook - hook a function directly in a vtable
*   VmtSwap - create a copy of a vtable and swap the pointer on the class instance

Hooking implementation files are located in [Hooks](https://github.com/danielkrupinski/Osiris/tree/master/Osiris/Hooks) directory.

## Acknowledgments

*   [ocornut](https://github.com/ocornut) and [contributors](https://github.com/ocornut/imgui/graphs/contributors) for creating and maintaining an amazing GUI library - [Dear imgui](https://github.com/ocornut/imgui).
*   [Zer0Mem0ry](https://github.com/Zer0Mem0ry) - for great tutorials on reverse engineering and game hacking

## License

> Copyright (c) 2018-2022 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](https://github.com/danielkrupinski/Osiris/blob/master/LICENSE) file for details.

## See also
*   [Anubis](https://github.com/danielkrupinski/Anubis) - free and open source cheat for CS:GO
*   [GOESP](https://github.com/danielkrupinski/GOESP) - free and open source cross-platform streamproof ESP hack for Counter-Strike: Global Offensive, written in modern C++
