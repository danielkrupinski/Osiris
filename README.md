# Osiris

![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)
![Linux](https://github.com/danielkrupinski/Osiris/workflows/Linux/badge.svg?branch=master&event=push)

Cross-platform (Windows, Linux) game hack for **Counter-Strike 2** with GUI and rendering based on game's Panorama UI. Compatible with the latest game update on Steam.

## What's new

* 20 May 2024 - Implemented showing players' active weapon ammo

![Player active weapon ammo](https://github.com/danielkrupinski/Osiris/assets/34515229/9a6dfc37-ee9f-4b70-9b1f-0e4465bf58fb)

* 15 May 2024 - Implemented showing an icon on player blinded by a flashbang

![Player blinded by flashbang](https://github.com/danielkrupinski/Osiris/assets/34515229/18b10e66-545a-449f-8783-691d5c1b2003)

* 14 May 2024 - Implemented showing an icon on player rescuing a hostage

![Player rescuing hostage](https://github.com/danielkrupinski/Osiris/assets/34515229/057b6dc7-1b54-44c0-9443-6917d2394335)

* 7 May 2024 - Implemented showing an icon on player picking up a hostage

![Player picking up hostage](https://github.com/danielkrupinski/Osiris/assets/34515229/d3a27f1a-dd79-4d18-bfbb-d8bb8c47ae2d)

* 6 May 2024 - Adjusted "Bomb timer" appearance. Implemented showing an icon on player defusing the bomb

![Player defusing icon](https://github.com/danielkrupinski/Osiris/assets/34515229/4addfc99-27d8-4f9d-a1b7-eb2b7c7565bd)

## Technical features

* C++ runtime library (CRT) is not used in release builds
* No heap memory allocations
* No static imports in release build on Windows
* No threads are created
* Exceptions are not used
* No external dependencies

## Compiling

### Prerequisites

#### Windows

* **Microsoft Visual Studio 2022** with **Desktop development with C++** workload

#### Linux

* **CMake 3.24** or newer
* **g++ 11 or newer** or **clang++ 15 or newer**

### Compiling from source

#### Windows

Open **Osiris.sln** in Visual Studio 2022, set build configuration to **Release | x64**. Press *Build solution* and you should receive **Osiris.dll** file.

#### Linux

Configure with CMake:

    cmake -DCMAKE_BUILD_TYPE=Release -B build

Build:

    cmake --build build -j $(nproc --all)

After following these steps you should receive **libOsiris.so** file in **build/Source/** directory.

### Loading / Injecting into game process

#### Windows

You need a **DLL injector** to inject (load) **Osiris.dll** into game process.

Counter-Strike 2 blocks LoadLibrary injection method, so you have to use a manual mapping (aka reflective DLL injection) injector.

**Xenos** and **Extreme Injector** are known to be **detected** by VAC.

#### Linux

You can simply run the following script in the directory containing **libOsiris.so**:

    sudo gdb -batch-silent -p $(pidof cs2) -ex "call (void*)dlopen(\"$PWD/libOsiris.so\", 2)"

However, this injection method might be detected by VAC as gdb is visible under **TracerPid** in `/proc/$(pidof cs2)/status` for the duration of the injection.

## License

> Copyright (c) 2018-2024 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](https://github.com/danielkrupinski/Osiris/blob/master/LICENSE) file for details.
