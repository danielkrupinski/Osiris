# Osiris

![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)
![Linux](https://github.com/danielkrupinski/Osiris/workflows/Linux/badge.svg?branch=master&event=push)

Free open-source cross-platform cheat software for **Counter-Strike 2** game. Designed as an internal cheat - [Dynamic-link library](https://en.wikipedia.org/wiki/Dynamic-link_library) (DLL) loadable into game process. Compatible with the Steam version of the game. Available for Windows and Linux systems.

## Technical features

* C++ runtime library (CRT) is not used in release builds
* No heap memory allocations
* No static imports in release build on Windows

## Compiling

### Prerequisites

<details>

<summary>Windows</summary>

* Microsoft Visual Studio 2022 with Windows SDK

</details>

<details>

<summary>Linux</summary>

* CMake 3.16 or newer
* **gcc 11 or newer** or **clang 13 or newer**

</details>

### Compiling from source

<details>

<summary>Windows</summary>

Open **Osiris.sln** in Visual Studio 2022, set build configuration to `Release | x64`. Press **Build solution** and you should receive `Osiris.dll` binary file.

</details>

<details>

<summary>Linux</summary>

Configure with CMake:

    cmake -DCMAKE_BUILD_TYPE=Release -B build

Build:

    cmake --build build -j $(nproc --all)

After following these steps you should receive `libOsiris.so` file in `build` directory.

</details>

### Loading / Injecting into game process

<details>

<summary>Windows</summary>

You need a **DLL injector** to inject (load) `Osiris.dll` into game process.

</details>

<details>

<summary>Linux</summary>

You can simply run the following script in the directory containing `libOsiris.so`:

    sudo gdb -batch-silent -p $(pidof cs2) -ex "call (void*)dlopen(\"$PWD/libOsiris.so\", 2)"

However, this injection method might be detected by VAC.

</details>

## License

> Copyright (c) 2018-2023 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](https://github.com/danielkrupinski/Osiris/blob/master/LICENSE) file for details.
