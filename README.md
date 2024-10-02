# Osiris

![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)
![Linux](https://github.com/danielkrupinski/Osiris/workflows/Linux/badge.svg?branch=master&event=push)

Cross-platform (Windows, Linux) game hack for **Counter-Strike 2** with GUI and rendering based on game's Panorama UI. Compatible with the latest game update on Steam.

## What's new

* 2 October 2024
    * Added bomb carrier icon to "Player Info in World" feature

    ![Bomb carrier icon](https://github.com/user-attachments/assets/ef903af4-20e7-4fa8-805f-fd825c672e66)

* 26 September 2024
    * Added outline glow for hostages

    ![Hostage glow](https://github.com/user-attachments/assets/e78fbebe-55df-42e2-bfc4-fb0cba3dbd30)

* 25 September 2024
    * Added outline glow for ticking bomb

    ![Ticking bomb glow](https://github.com/user-attachments/assets/ddc4063e-accb-44e8-b202-05a0bf2c49ae)

* 23 September 2024
    * Added outline glow for dropped bomb

    ![Dropped bomb glow](https://github.com/user-attachments/assets/080043c2-bff2-444e-983a-a89b275a553b)

* 20 September 2024
    * Added outline glow for grenade projectiles

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
