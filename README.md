# SyringeRailControl

Firmware for an STM32F407 Discovery board (`STM32F407VGT6`) controlling a two-axis stepper motion system:

- **Rail Axis**: Controls horizontal linear carriage translation for positioning.
- **Syringe Axis**: Controls plunger displacement for liquid dispensing and aspiration.

Each axis is powered by a 28BYJ-48 unipolar stepper motor driven through a ULN2003A Darlington array.

---

## Hardware Overview

### Required Components

- **MCU Board**: STM32F407G-DISC1 (or compatible STM32F407VGT6 board)
- **Motors**: 2x 28BYJ-48 geared stepper motors (5V)
- **Motor Drivers**: 2x ULN2003A driver boards
- **Speed Inputs**: 2x 10kΩ linear potentiometers
- **Limit Switches**: 4x momentary microswitches (normally open or normally closed to GND)
- **Control Inputs**: 4x momentary pushbuttons (Start/Stop for each axis), 2x momentary pushbuttons (Rail Jog Left/Right), 2x toggle switches (Direction selectors)
- **Power Supply**: External 5V DC power supply for the stepper drivers

> **Important**: Do not power the stepper motors directly from the STM32 board headers. Each 28BYJ-48 motor can draw up to 300 mA under load, which can cause voltage brownouts and reset the microcontroller. Power the ULN2003 boards from a dedicated 5V power supply and connect the external power supply GND to the STM32 GND.

---

## Wiring & Pin Assignments

### Stepper Motor Drivers (ULN2003A)

Outputs are configured as GPIO Push-Pull (`Low Speed`, `No-Pull`).

| Signal | STM32 Pin | Driver Pin | Motor Phase (Typical 28BYJ-48) |
|---|---|---|---|
| **Rail Phase 0** | `PD0` | IN1 | Blue (Coil 1) |
| **Rail Phase 1** | `PD1` | IN2 | Pink (Coil 2) |
| **Rail Phase 2** | `PD2` | IN3 | Yellow (Coil 3) |
| **Rail Phase 3** | `PD3` | IN4 | Orange (Coil 4) |
| **Syringe Phase 0** | `PD4` | IN1 | Blue (Coil 1) |
| **Syringe Phase 1** | `PD5` | IN2 | Pink (Coil 2) |
| **Syringe Phase 2** | `PD6` | IN3 | Yellow (Coil 3) |
| **Syringe Phase 3** | `PD7` | IN4 | Orange (Coil 4) |

Driver board power pins:
- `VCC` -> External +5V supply
- `GND` -> External 5V supply GND **and** STM32 GND

---

### Analog Speed Potentiometers

Potentiometer outer terminals connect to `3.3V` and `GND`. Center wipers connect to ADC inputs.

| Signal | STM32 Pin | ADC Channel | Delay Range | Description |
|---|---|---|---|---|
| **Rail Speed** | `PA1` | ADC1_IN1 (Rank 1) | 1 ms – 20 ms | Controls carriage traversal speed |
| **Syringe Speed** | `PA2` | ADC1_IN2 (Rank 2) | 1 ms – 20 ms | Controls plunger feed / withdrawal speed |

---

### Control Panel & Limit Switches

All digital control inputs are on `GPIOE` with internal pull-ups enabled. Switches should pull the pin to `GND` when activated (Active-Low).

| Pin | Identifier | Type | Function |
|---|---|---|---|
| `PE0` | `BTN_RAIL_START` | Momentary Button | Start rail carriage motion |
| `PE1` | `BTN_RAIL_STOP` | Momentary Button | Stop rail carriage motion |
| `PE2` | `SW_RAIL_DIR` | Toggle Switch | Low = Forward / Right, High = Reverse / Left |
| `PE3` | `BTN_RAIL_FFW_L` | Momentary Button | Jog rail left at maximum speed (1 ms delay) |
| `PE4` | `BTN_RAIL_FFW_R` | Momentary Button | Jog rail right at maximum speed (1 ms delay) |
| `PE5` | `SW_RAIL_LIM_L` | Limit Switch | Left rail endstop |
| `PE6` | `SW_RAIL_LIM_R` | Limit Switch | Right rail endstop |
| `PE7` | `BTN_SYR_START` | Momentary Button | Start syringe plunger motion |
| `PE8` | `BTN_SYR_STOP` | Momentary Button | Stop syringe plunger motion |
| `PE9` | `SW_SYR_DIR` | Toggle Switch | Low = Infuse (extend), High = Withdraw (retract) |
| `PE10` | `SW_SYR_LIM_T` | Limit Switch | Top syringe endstop (maximum extension) |
| `PE11` | `SW_SYR_LIM_B` | Limit Switch | Bottom syringe endstop (maximum retraction) |

---

## Operating Behavior

### Stepping Sequence
The firmware uses an 8-step half-stepping sequence:
```
Step 0: [1, 0, 0, 0]
Step 1: [1, 1, 0, 0]
Step 2: [0, 1, 0, 0]
Step 3: [0, 1, 1, 0]
Step 4: [0, 0, 1, 0]
Step 5: [0, 0, 1, 1]
Step 6: [0, 0, 0, 1]
Step 7: [1, 0, 0, 1]
```
Half-stepping doubles the angular resolution (4096 half-steps per shaft revolution on a standard 64:1 reduction 28BYJ-48) and provides smoother low-speed rotation.

### Speed Mapping
The 12-bit ADC readings (0 to 4095) map linearly to millisecond step intervals:

$$\text{delay (ms)} = 1 + \left\lfloor \frac{\text{ADC} \times 19}{4095} \right\rfloor$$

- `ADC = 0` produces a 1 ms step delay (highest speed).
- `ADC = 4095` produces a 20 ms step delay (lowest speed).

### Safety Interlocks & Direction Recovery
When a limit switch is triggered:
- The motor stops immediately if trying to move into the limit.
- Motion in the **opposite** direction is still permitted. Flipping the direction toggle switch allows backing off the switch without manual intervention.

### Manual Jog Mode
Pressing `BTN_RAIL_FFW_L` or `BTN_RAIL_FFW_R` overrides normal button states and potentiometer speeds, driving the rail at maximum speed (1 ms delay) for quick alignment.

### Thermal Protection
When a motor stops (`Stepper_Stop()`), all four driver lines are driven low (`GPIO_PIN_RESET`). This shuts off current to the motor coils, preventing the 28BYJ-48 and ULN2003 from heating up while idle.

---

## Project Structure

```
SyringeRailControl/
├── LICENSE                          # MIT License
├── README.md                        # Documentation and wiring guide
├── .gitignore                       # Build and environment exclusions
└── SyringeRailController/
    ├── CMakeLists.txt               # Cross-compilation build definition
    ├── build.sh                     # Build helper script
    ├── STM32F407VGTX_FLASH.ld       # Linker script
    ├── startup_stm32f407vgtx.s      # Cortex-M4 vector table and startup code
    ├── Core/
    │   ├── Inc/
    │   │   ├── app.h                # Hardware pin definitions and application API
    │   │   ├── stepper.h            # Stepper driver struct and prototypes
    │   │   ├── main.h               # Core includes and clock prototypes
    │   │   ├── stm32f4xx_hal_conf.h # HAL peripheral driver configuration
    │   │   └── stm32f4xx_it.h       # ISR declarations
    │   └── Src/
    │       ├── app.c                # Control logic, ADC mapping, and limit interlocks
    │       ├── stepper.c            # Half-stepping engine and timing logic
    │       ├── main.c               # System clock, GPIO/ADC setup, and polling loop
    │       ├── stm32f4xx_it.c       # SysTick and exception handlers
    │       └── system_stm32f4xx.c   # CMSIS system initialization
    └── Drivers/
        ├── CMSIS/                   # ARM Cortex-M4 CMSIS headers
        └── STM32F4xx_HAL_Driver/    # STMicroelectronics HAL drivers
```

---

## Build and Flashing

### Prerequisites

Install the ARM GCC toolchain and build tools:

- **Toolchain**: `arm-none-eabi-gcc`
- **Build System**: `cmake` (version 3.22 or higher)
- **Make**: `make` or `ninja`
- **Programmer**: `openocd` or `stlink` (`st-flash`)

*(If using STM32CubeCLT, all of these tools are included in `/opt/ST/STM32CubeCLT_x.x.x`)*

### Building

Using the build script:
```bash
cd SyringeRailController
chmod +x build.sh
./build.sh
```

Or building directly with CMake:
```bash
cd SyringeRailController
mkdir -p build && cd build
cmake .. -G "Unix Makefiles"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
```

The build generates the following files in `SyringeRailController/build/`:
- `SyringeRailController.elf`
- `SyringeRailController.bin`
- `SyringeRailController.hex`

### Flashing to STM32

Connect your STM32F4 Discovery board via the ST-LINK USB port (USB Mini-B connector).

**Using CMake/Make target:**
```bash
cd SyringeRailController/build
make flash
```

**Using OpenOCD directly:**
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program SyringeRailController.bin 0x08000000 verify reset exit"
```

**Using st-flash:**
```bash
st-flash write SyringeRailController.bin 0x08000000
```

---

## Configuration & Tuning

To adjust hardware pin assignments or timing behavior, edit [`Core/Inc/app.h`](SyringeRailController/Core/Inc/app.h) and [`Core/Src/app.c`](SyringeRailController/Core/Src/app.c):

- **Pin Assignments**: All button, switch, and motor output ports and pins are defined as `#define` constants in `app.h`.
- **Button Polarity**: If using active-high buttons (external pull-downs), change `#define BTN_PRESSED GPIO_PIN_RESET` to `GPIO_PIN_SET` in `app.c`.
- **Speed Limits**: To adjust the speed range, modify `map_adc_to_delay()` in `app.c`. The minimum delay is clamped to 1 ms to prevent motor stalling.

---

## License

This project is open-source under the [MIT License](LICENSE).
