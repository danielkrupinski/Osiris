import os
import subprocess

# Define the path to the shared object
lib_path = os.path.join(os.getcwd(), "build/Source/libOsiris.so")

# Try to get the PID of the cs2 process
try:
    pid = subprocess.check_output(["pidof", "cs2"]).decode().strip()
except subprocess.CalledProcessError:
    pid = None

# Define the menu options
menu = {
    "1": f"sudo gdb -batch -n -q -p <PID> -ex \"call (void*)dlopen(\\\"{lib_path}\\\", 2)\"",
    "2": f"sudo injectso <PID> \"{lib_path}\"",
    "3": f"LD_PRELOAD=\"{lib_path}\" ./cs2",
    "4": f"sudo gdb -batch -n -q --args ./cs2 -ex \"set environment LD_PRELOAD={lib_path}\" -ex run",
    "5": f"sudo gdb -batch -n -q -p {pid if pid else '<PID>'} -ex \"call (void*)dlopen(\\\"$(realpath {lib_path})\\\", 2)\"",
    "6": f"sudo gdb -batch-silent -p {pid if pid else '<PID>'} -ex \"call (void*)dlopen(\\\"{lib_path}\\\", 2)\""
}

# Display the menu
print("Injection Method Menu:\n")
for key, command in menu.items():
    print(f"{key}. {command}")

if pid:
    print(f"\nDetected cs2 PID: {pid}")
else:
    print("\ncs2 process not detected. Please start cs2 or replace <PID> manually.")
