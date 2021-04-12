#!/bin/bash

gdb="$(dirname "$0")/gdb" # For using a gdb build such as the cathook one (The one included)
libname="libgamemode.so"     # Pretend to be gamemode, change this to another lib that may be in /maps
csgo_pid=$(pidof csgo_linux64)

export CC="gcc"
export CXX="g++"

if [[ $EUID -eq 0 ]]; then
  echo "You cannot run this as root."
  exit 1
fi

rm -rf /tmp/dumps
mkdir -p --mode=000 /tmp/dumps

function unload() {
  echo "Unloading cheat..."
  echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
  if grep -q "$libname" "/proc/$csgo_pid/maps"; then
    $gdb -n -q -batch -ex "attach $csgo_pid" \
      -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
      -ex "set \$dlclose = (int(*)(void*)) dlclose" \
      -ex "set \$library = \$dlopen(\"$(pwd)/cmake-build-release/$libname\", 6)" \
      -ex "call \$dlclose(\$library)" \
      -ex "call \$dlclose(\$library)" \
      -ex "detach" \
      -ex "quit"

    $gdb -n -q -batch -ex "attach $csgo_pid" \
      -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
      -ex "set \$dlclose = (int(*)(void*)) dlclose" \
      -ex "set \$library = \$dlopen(\"$(pwd)/cmake-build-debug/$libname\", 6)" \
      -ex "call \$dlclose(\$library)" \
      -ex "call \$dlclose(\$library)" \
      -ex "detach" \
      -ex "quit"
  fi
  echo "Unloaded!"
}

function load() {
  echo "Loading cheat..."

  if [[ -z "$csgo_pid" ]]; then
    steam steam://rungameid/730 >/dev/null 2>&1 &
    disown
    while [ -z "$csgo_pid" ]; do
      csgo_pid=$(pidof csgo_linux64)
    done
  fi

  echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
  yes | \cp -rf cmake-build-release/libOsiris.so cmake-build-release/$libname
  sudo killall -19 steam
  sudo killall -19 steamwebhelper
  $gdb -n -q -batch \
    -ex "set auto-load safe-path $(pwd)/build/:/usr/lib/" \
    -ex "attach $csgo_pid" \
    -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
    -ex "call \$dlopen(\"$(pwd)/cmake-build-release/$libname\", 1)" \
    -ex "detach" \
    -ex "quit"
  sleep 1
  sudo killall -18 steamwebhelper
  sudo killall -18 steam
  echo "Successfully loaded!"
}

function load_debug() {
  echo "Loading cheat..."
  if [[ -z "$csgo_pid" ]]; then
    steam steam://rungameid/730 >/dev/null 2>&1 &
    disown
    while [ -z "$csgo_pid" ]; do
      csgo_pid=$(pidof csgo_linux64)
    done
  fi
  echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
  yes | \cp -rf cmake-build-debug/libOsiris.so cmake-build-debug/$libname
  sudo killall -19 steam
  sudo killall -19 steamwebhelper
  $gdb -n -q -batch \
    -ex "set auto-load safe-path $(pwd)/build/:/usr/lib/" \
    -ex "attach $csgo_pid" \
    -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
    -ex "call \$dlopen(\"$(pwd)/cmake-build-debug/$libname\", 1)" \
    -ex "detach" \
    -ex "quit"
  $gdb -p "$csgo_pid"
  sudo killall -18 steamwebhelper
  sudo killall -18 steam
  echo "Successfully loaded!"
}

function build() {
  echo "building cheat..."
  if [[ ! -d "cmake-build-release" ]]; then
    mkdir cmake-build-release
  fi
  cd cmake-build-release
  cmake -D CMAKE_BUILD_TYPE=Release ..
  make -j $(nproc --all)
  cd ..
}

function build_debug() {
  echo "building cheat in debug..."
  if [[ ! -d "cmake-build-debug" ]]; then
    mkdir cmake-build-debug
  fi
  cd cmake-build-debug
  cmake -D CMAKE_BUILD_TYPE=Debug ..
  make -j $(nproc --all)
  cd ..
}

function pull() {
  git pull
}

function zombies() {
  if ! [[ -z $(pidof gdb) ]]; then
    sudo killall -9 gdb
  fi

  if ! [[ -z $(pidof steam) ]]; then
    sudo killall -9 steam
  fi

  if ! [[ -z $(pidof steamwebhelper) ]]; then
    sudo killall -9 steamwebhelper
  fi

  if ! [[ -z $(pidof SteamChildMonit) ]]; then
    sudo killall -9 SteamChildMonit
  fi

  if ! [[ -z $(pidof gameoverlayui) ]]; then
    sudo killall -9 gameoverlayui
  fi

  if ! [[ -z $(pidof csgo.sh) ]]; then
    sudo killall -9 csgo.sh
  fi

  if ! [[ -z $(pidof csgo_linux64) ]]; then
    sudo killall -9 csgo_linux64
  fi
}

while [[ $# -gt 0 ]]; do
  keys="$1"
  case $keys in
  -u | --unload)
    unload
    shift
    ;;
  -l | --load)
    load
    shift
    ;;
  -ld | --load_debug)
    load_debug
    shift
    ;;
  -b | --build)
    build
    shift
    ;;
  -bd | --build_debug)
    build_debug
    shift
    ;;
  -p | --pull)
    pull
    shift
    ;;
  -z | --zombies)
    zombies
    shift
    ;;
  -h | --help)
    echo "
 help
Toolbox script for Osiris
=======================================================================
| Argument             | Description                                  |
| -------------------- | -------------------------------------------- |
| -u (--unload)        | Unload the cheat from CS:GO if loaded.       |
| -l (--load)          | Load/inject the cheat via gdb.               |
| -ld (--load_debug)   | Load/inject the cheat and debug via gdb.     |
| -b (--build)         | Build to the build/ dir.                     |
| -bd (--build_debug)  | Build to the build/ dir as debug.            |
| -p (--pull)          | Update the cheat.                            |
| -z (--zombies)       | Kill zombie processes.                       |
| -h (--help)          | Show help.                                   |
=======================================================================

All args are executed in the order they are written in, for
example, \"-p -u -b -l\" would update the cheat, then unload, then build it, and
then load it back into csgo.
"
    exit
    ;;
  *)
    echo "Unknown arg $1, use -h for help"
    exit
    ;;
  esac
done
