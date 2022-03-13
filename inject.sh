#!/usr/bin/env bash
export PATH="$HOME/.local/bin:$PATH"

function echo_green {
	echo -e "\\e[32m$*\\e[0m"
}

function echo_orange {
	echo -e "\\e[33m$*\\e[0m"
}

function echo_red {
	echo -e "\\e[31m$*\\e[0m"
}

csgo_pid=$(pidof csgo_linux64)
if [ -z "$csgo_pid" ]; then
    echo_red "CS:GO needs to be open before you can inject, exiting..."
    exit 1
fi

# pBypass for crash dumps being sent
# You may also want to consider using -nobreakpad in your launch options.
sudo rm -rf /tmp/dumps # Remove if it exists
sudo mkdir --mode=000 /tmp/dumps # Make it as root with no permissions

filename="libOsiris.so"

# Credit: Aixxe @ aixxe.net
if grep -q "$filename" /proc/"$csgo_pid"/maps; then
    echo_orange "Osiris is already injected, aborting..."
    exit
fi

echo "Injecting Build ID: $filename"

# https://www.kernel.org/doc/Documentation/security/Yama.txt
echo "2" | sudo tee /proc/sys/kernel/yama/ptrace_scope # Only allows root to inject code. This is temporary until reboot.

sudo cp "$filename" "/usr/lib/${filename}"

sudo killall -19 steam
sudo killall -19 steamwebhelper

lib_dir_name="lib"
if [ $(getconf LONG_BIT) = 64 ]; then
    lib_dir_name+="64"
fi

input="$(
sudo gdb -n -q -batch-silent \
  -ex "set logging on" \
  -ex "set logging file /dev/null" \
  -ex "set logging redirect on" \
  -ex "set auto-load safe-path /usr/share/gdb/auto-load/usr/$lib_dir_name/:/usr/$lib_dir_name/" \
  -ex "attach $csgo_pid" \
  -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
  -ex "call \$dlopen(\"/usr/lib/$filename\", 1)" \
  -ex "detach" \
  -ex "quit"
)"
sleep 1
sudo killall -18 steamwebhelper
sudo killall -18 steam

sudo rm "/usr/lib/${filename}"

last_line="${input##*$'\n'}"

if [ "$last_line" != "\$1 = (void *) 0x0" ]; then
    echo_green "Successfully injected!"
else
    echo_red "Injection failed, make sure you have compiled..."
fi
