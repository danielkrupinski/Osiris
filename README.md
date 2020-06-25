# Osiris - โอซีริส
[![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=plastic)](https://en.wikipedia.org/wiki/C%2B%2B) 
[![CS:GO](https://img.shields.io/badge/game-CS%3AGO-yellow.svg?style=plastic)](https://store.steampowered.com/app/730/CounterStrike_Global_Offensive/) 
[![Windows](https://img.shields.io/badge/platform-Windows-0078d7.svg?style=plastic)](https://en.wikipedia.org/wiki/Microsoft_Windows) 
[![x86](https://img.shields.io/badge/arch-x86-red.svg?style=plastic)](https://en.wikipedia.org/wiki/X86) 
[![License](https://img.shields.io/github/license/danielkrupinski/Osiris.svg?style=plastic)](LICENSE)
[![Issues](https://img.shields.io/github/issues/danielkrupinski/Osiris.svg?style=plastic)](https://github.com/danielkrupinski/Osiris/issues)
[![PayPal](https://img.shields.io/badge/donate-PayPal-104098.svg?style=plastic&logo=PayPal)](https://paypal.me/DanielK19)
![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)

ซอฟต์แวร์โอเพนซอร์ซฟรี สำหรับโกงสำหรับเกม Counter-Strike: Global Offensive เข้ากันได้กับเกมเวอร์ชั่นล่าสุดบน Steam

## ฟีเจอร์
* **ล็อคเป้า**
* **ยิงอัตโนมัติ**
* **ยิงซ้ำรอย**
* **ตัวละครเรืองแสง**
* **สีตัวละคร**
* **รายละเอียดตัวละคร**
* **การมองเห็น**
* **เปลี่ยนสกินปืน**
* **ปรับเสียงในเกม**
* **ธีม**
* **อื่นๆ**
* **รีพอร์ต**
* **คอนฟิก**

## Getting started

### Prerequisites
Microsoft Visual Studio 2019 (preferably the latest version), platform toolset v142 and Windows SDK 10.0 are required in order to compile Osiris. If you don't have ones, you can download VS [here](https://visualstudio.microsoft.com/) (Windows SDK is installed during Visual Studio Setup).

### Downloading

There are two options of downloading the source code:

#### Without [git](https://git-scm.com)

Choose this option if you want pure source and you're not going to contribute to the repo. Download size ~600 kB.

To download source code this way [click here](https://github.com/danielkrupinski/Osiris/archive/master.zip).

#### With [git](https://git-scm.com)

Choose this option if you're going to contribute to the repo or you want to use version control system. Download size ~100 MB (because of full commit history). Git is required to step further, if not installed download it [here](https://git-scm.com).

Open git command prompt and enter following command:
```
git clone https://github.com/danielkrupinski/Osiris.git
```
`Osiris` folder should have been succesfully created, containing all the source files.

### Compiling from source

When you have equiped a copy of source code, next step is opening **Osiris.sln** in Microsoft Visual Studio 2019.

Then change build configuration to `Release | x86` and simply press **Build solution**.

If everything went right you should receive `Osiris.dll`  binary file.

### Loading / Injecting into game process

Open your favorite [DLL injector](https://en.wikipedia.org/wiki/DLL_injection) and just inject `Osiris.dll` into `csgo.exe` process.

When injected, menu is openable under `INSERT` key.

### Further optimizations
If your CPU supports AVX / AVX2 instruction set, you can enable it in project settings. This should result in more performant code, optimized for your CPU. Currently SSE2 instructions are selected in project settings.

## FAQ

### How do I open menu?
Press <kbd>INSERT</kbd> while focused on CS:GO window.

### Where is my config file saved?
Configuration files are saved inside `Osiris` folder in your `Documents` folder (`%USERPROFILE%\Documents\Osiris`). The config is in human readable format and can be edited (e.g, using notepad). Sometimes after updates configuration file needs to be deleted and recreated.

### What is cheat execution entry point and where is it located?
Osiris is based on six main global objects ("modules") - see [Osiris.cpp](https://github.com/danielkrupinski/Osiris/blob/master/Osiris/Osiris.cpp) file for detailes. The modules are initialized in the order they are declared in above file as they depend on each other.

## Acknowledgments

* [ocornut](https://github.com/ocornut) and [contributors](https://github.com/ocornut/imgui/graphs/contributors) for creating such amazing GUI library - [imgui](https://github.com/ocornut/imgui).
* [Zer0Mem0ry](https://github.com/Zer0Mem0ry) - for great tutorials on reverse engineering and game hacking

## License

> Copyright (c) 2018-2020 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](LICENSE) file for details.

## See also
- [Anubis](https://github.com/danielkrupinski/Anubis) - free and open source cheat for CS:GO with configuration compatible with Osiris
- [GOESP](https://github.com/danielkrupinski/GOESP) - free and open source stream-proof ESP hack for Counter-Strike: Global Offensive, written in modern C++
