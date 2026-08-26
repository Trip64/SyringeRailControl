# Syringe Rail Controller Firmware

STM32F407VGT6 firmware source for the dual-axis syringe and rail controller.

See [Root README](../README.md) for complete pinout tables and wiring specifications.

## Build

```bash
./build.sh
```

Or manually:

```bash
mkdir -p build && cd build
cmake .. -G "Unix Makefiles"
make -j4
make flash
```
