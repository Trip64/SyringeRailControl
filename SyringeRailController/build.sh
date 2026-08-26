#!/bin/bash
set -e

# Go to the root of the project where build.sh is located
cd "$(dirname "$0")"

# Inject STM32CubeCLT tools into PATH (CMake, Ninja, GCC)
export PATH="/opt/ST/STM32CubeCLT_1.20.0/CMake/bin:/opt/ST/STM32CubeCLT_1.20.0/Ninja/bin:/opt/ST/STM32CubeCLT_1.20.0/GNU-tools-for-STM32/bin:$PATH"

echo "==> Configuring Project <=="
mkdir -p build
cd build
rm -f CMakeCache.txt
cmake .. -G "Unix Makefiles"

echo "==> Compiling <=="
# Use Ninja if available, otherwise fallback to make
if command -v ninja >/dev/null 2>&1; then
    cmake --build . -- -j$(sysctl -n hw.ncpu)
else
    make -j$(sysctl -n hw.ncpu)
fi

echo "==> Done! <=="
echo "You can flash the binary by running:"
echo "cd build && make flash"
