# Osiris

Free open-source cheating software for Counter-Strike: Global Offensive. Designed as internal cheat - injectable DLL.

## Features

* Glow - render glow effect on entities
    * Enable - on / off master switch
    * Players - enable glow on players
    * Enemies only - glow only oponents
    * Thickness - outline thickness
    * Alpha - outline alpha
    * Style - glow style [0-3]
    * Allies color
    * Enemies color
    * Weapons - enable glow on weapon
    * Weapons color
    * Chickens - enable glow on chickens
    * Chickens color

* Misc
    * Bunny hop - automatically jump to increase movement speed
    * Disable post-processing - disable post-processing effects in order to increase FPS
    * Skybox - change skybox

* Config
    * Load - load configuration file
    * Save - save configuration file
    * Reset - restore default configuration settings

## Getting started

### Prerequisites
Microsoft Visual Studio 2017 and Windows SDK 10.0.17134.0 are required in order to compile Osiris. If you don't have ones, you can download VS [here](https://visualstudio.microsoft.com/) (Windows SDK is installed during Visual Studio Setup).

## FAQ

### Where is my config file saved?
Configuration file is saved under name `Osiris.json` in your `Documents` folder (`%USERPROFILE%\Documents`).

## License

> Copyright (c) 2018 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](LICENSE) file for details.
