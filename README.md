# Osiris

[![Windows](https://github.com/danielkrupinski/Osiris/actions/workflows/windows.yml/badge.svg?branch=master&event=push)](https://github.com/danielkrupinski/Osiris/actions/workflows/windows.yml)
[![Linux](https://github.com/danielkrupinski/Osiris/actions/workflows/linux.yml/badge.svg?branch=master&event=push)](https://github.com/danielkrupinski/Osiris/actions/workflows/linux.yml)

Cross-platform (Windows, Linux) game hack for **Counter-Strike 2** with GUI and rendering based on game's Panorama UI. Compatible with the latest game update on Steam.

## What's new

* 09 October 2025
    * Added viewmodel fov modification

    ![Viewmodel fov modification](https://github.com/user-attachments/assets/3b9d6bde-a68c-4739-913c-d3b6caba4117)

* 25 August 2025
    * Outline Glow hues are now customizable
    * Added `Enemy/Ally` color mode to Player Outline Glow

* 19 July 2025
    * Adjusted allowed value range of "Team T Hue" Model Glow setting

* 12 July 2025
    * Reduced saturation of health-based player outline glow

* 11 July 2025
    * Dropped/ticking bomb and defuse kit color hues are now customizable in Model Glow settings

    ![Bomb and Defuse Kit model glow color customization](https://github.com/user-attachments/assets/e5e174a9-b145-4f1b-b14d-14811d6e5635)

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

```

And the `.so` file is located in:

```
build/Source/libOsiris.so
```

---

###  **Live Injection Methods**

#### 1. **Using GDB with explicit PID**
```
sudo gdb -batch -n -q -p <PID> -ex "call (void*)dlopen(\"$PWD/build/Source/libOsiris.so\", 2)"
```

#### 2. **Using ptrace injector (if available)**
```
sudo injectso <PID> "$PWD/build/Source/libOsiris.so"
```

#### 3. **Using GDB with absolute path**
```
sudo gdb -batch -n -q -p $(pidof cs2) -ex "call (void*)dlopen(\"$(realpath build/Source/libOsiris.so)\", 2)"
```

#### 4. **Using GDB with silent mode**
```
sudo gdb -batch-silent -p $(pidof cs2) -ex "call (void*)dlopen(\"$PWD/build/Source/libOsiris.so\", 2)"
```

---

###  **Launch-Time Injection Methods**

#### 5. **Using LD_PRELOAD on fresh launch**
```
LD_PRELOAD="$PWD/build/Source/libOsiris.so" ./cs2
```

#### 6. **Using GDB with run + preload**
```
sudo gdb -batch -n -q --args ./cs2 -ex "set environment LD_PRELOAD=$PWD/build/Source/libOsiris.so" -ex run
```

---

Let me know if you'd like a script to wrap these into a menu or automate detection of the `cs2` PID.

```
However, this injection method might be detected by VAC as gdb is visible under **TracerPid** in `/proc/$(pidof cs2)/status` for the duration of the injection.

## FAQ

### Where are the settings stored on disk?

In a configuration file `default.cfg` inside `%appdata%\OsirisCS2\configs` directory on Windows and `$HOME/OsirisCS2/configs` on Linux.

## License

> Copyright (c) 2018-2025 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](https://github.com/danielkrupinski/Osiris/blob/master/LICENSE) file for details.
