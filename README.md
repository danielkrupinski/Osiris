# Osiris

[![Windows](https://github.com/danielkrupinski/Osiris/actions/workflows/windows.yml/badge.svg?branch=master&event=push)](https://github.com/danielkrupinski/Osiris/actions/workflows/windows.yml)
[![Linux](https://github.com/danielkrupinski/Osiris/actions/workflows/linux.yml/badge.svg?branch=master&event=push)](https://github.com/danielkrupinski/Osiris/actions/workflows/linux.yml)

Cross-platform (Windows, Linux) game hack for **Counter-Strike 2** with GUI and rendering based on game's Panorama UI. Compatible with the latest game update on Steam.

## What's new

* 05 May 2025
    * Player Model Glow hues are now customizable also for `Team`, `Health-based` and `Enemy/Ally` color modes

    ![Player Model Glow Hues](https://github.com/user-attachments/assets/2116b7e1-74b4-475c-b168-c712d396a62c)

* 29 April 2025
    * Player color hues are now customizable in Model Glow settings

    ![Custom player color hues in Model Glow](https://github.com/user-attachments/assets/6f543dd5-3d16-49d6-aca8-81b9a8be1ee8)

* 11 April 2025
    * Added "Enemy / Ally" color mode for Player Model Glow

    <br>
    <img src=https://github.com/user-attachments/assets/c5e3143c-e5af-40b2-8ebe-b8fa0e2034ca height="200" alt="Player Model Glow Enemy/Ally Color Mode">

* 7 April 2025
    * Added weapon model glow preview

    <img src=https://github.com/user-attachments/assets/409cabba-b80a-49d1-8737-d0178ad1ead9 height="200" alt="Weapon Model Glow Preview">

* 31 March 2025
    * Moved model glow preview to the left side of the model glow tab

    <img src=https://github.com/user-attachments/assets/9cd923b0-7f37-46e9-b857-d5ee0930a82b height="200" alt="Model Glow Preview">

* 29 March 2025
    * Moved "Unload" button to the right side of the tab bar

* 27 March 2025
    * Added an option to restore default settings in the active config file

    ![Restore Defaults](https://github.com/user-attachments/assets/496cc3cb-17ee-445a-802c-2f496f39a34c)

* 06 March 2025
    * Settings are now automatically saved to the config file (`default.cfg`) inside `%appdata%\OsirisCS2\configs` directory on Windows and `$HOME/OsirisCS2/configs` on Linux. `default.cfg` file is loaded automatically when starting Osiris.

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
* **g++ 11 or newer** or **clang++ 17 or newer**

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

It's possible to hide the TracerPid by applying the following patch to your kernel sources:

    --- a/fs/proc/array.c
    +++ b/fs/proc/array.c
    @@ -162,7 +162,7 @@
     
     	tracer = ptrace_parent(p);
     	if (tracer)
    -		tpid = task_pid_nr_ns(tracer, ns);
    +		tpid = 0;
     
     	tgid = task_tgid_nr_ns(p, ns);
     	ngid = task_numa_group_id(p);

## License

> Copyright (c) 2018-2024 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](https://github.com/danielkrupinski/Osiris/blob/master/LICENSE) file for details.
