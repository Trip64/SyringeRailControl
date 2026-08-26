# SyringeRailControl

Firmware for an STM32F407 Discovery board controlling a 2-axis stepper system:
1. **Rail Axis** (horizontal movement)
2. **Syringe Axis** (plunger movement)

Driven using 28BYJ-48 stepper motors with ULN2003 driver boards.

## Hardware Wiring

### Motors (ULN2003)

| Axis | Phase | STM32 Pin | Driver Pin |
|---|---|---|---|
| Rail | P0 | `PD0` | IN1 |
| Rail | P1 | `PD1` | IN2 |
| Rail | P2 | `PD2` | IN3 |
| Rail | P3 | `PD3` | IN4 |
| Syringe | P0 | `PD4` | IN1 |
| Syringe | P1 | `PD5` | IN2 |
| Syringe | P2 | `PD6` | IN3 |
| Syringe | P3 | `PD7` | IN4 |

*Note: Power ULN2003 boards from an external 5V source and tie GND to the STM32.*

### Speed Controls (Potentiometers)

10k linear potentiometers connected between 3.3V and GND:

- **Rail Speed**: `PA1` (ADC1 Channel 1)
- **Syringe Speed**: `PA2` (ADC1 Channel 2)

ADC values (0–4095) map to step delays between 1 ms (fastest) and 20 ms (slowest).

### Buttons and Limit Switches

All inputs use internal pull-ups on `GPIOE` (active-low to GND):

| Pin | Control | Description |
|---|---|---|
| `PE0` | Rail Start | Start normal rail movement |
| `PE1` | Rail Stop | Stop rail movement |
| `PE2` | Rail Direction | Toggle switch (Low = Forward, High = Reverse) |
| `PE3` | Rail Jog Left | Hold to jog left at max speed |
| `PE4` | Rail Jog Right | Hold to jog right at max speed |
| `PE5` | Rail Limit Left | Left endstop switch |
| `PE6` | Rail Limit Right | Right endstop switch |
| `PE7` | Syringe Start | Start syringe movement |
| `PE8` | Syringe Stop | Stop syringe movement |
| `PE9` | Syringe Direction | Toggle switch (Low = Infuse, High = Withdraw) |
| `PE10` | Syringe Limit Top | Top endstop switch |
| `PE11` | Syringe Limit Bottom | Bottom endstop switch |

## How It Works

- **Half-stepping**: Uses an 8-step sequence for smooth motion.
- **Limit protection**: When an endstop is hit, motion in that direction is blocked immediately while allowing movement in the opposite direction.
- **Fast-forward override**: Holding PE3 or PE4 runs the rail at maximum speed (1 ms delay) regardless of potentiometer setting.
- **Motor sleep**: Coils are turned off when stopped to avoid heating.

## Build and Flash

### Requirements
- `arm-none-eabi-gcc`
- `cmake` (>= 3.22)
- `make`
- `openocd`

### Build
```bash
cd SyringeRailController
./build.sh
```

Or manually:
```bash
cd SyringeRailController
mkdir -p build && cd build
cmake ..
make -j4
```

### Flash
```bash
cd SyringeRailController/build
make flash
```

Or using `openocd`:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program SyringeRailController.bin 0x08000000 verify reset exit"
```

## License
MIT
