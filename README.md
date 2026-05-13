# CHIP-8 Emulator

A CHIP-8 emulator written in C++ using SDL3. Built from scratch as a learning project covering emulator fundamentals including CPU emulation, display rendering, input handling, and audio.

---

## What is CHIP-8?

CHIP-8 is an interpreted programming language developed in the mid-1970s by Joseph Weisbecker, designed to run on the COSMAC VIP microcomputer. It was never a physical chip — it is a virtual machine specification with 35 opcodes. It is widely considered the ideal starting point for learning emulator development.

---

## Features

- Full implementation of all 35 standard CHIP-8 opcodes
- 64×32 monochrome display rendered via SDL3, scaled 10× to 640×320
- 16-key hexadecimal keypad input
- Delay and sound timer running at 60 Hz
- Square wave audio beep via SDL3 AudioStream
- Configurable CPU cycle rate (default: 10 cycles per frame)
- ROM loading from file

---

## Requirements

- C++20 or later
- SDL3
- CMake
- A MinGW or compatible compiler (project developed with MinGW via CLion)

---

## Building

```bash
git clone https://github.com/yourusername/chip8.git
cd chip8
mkdir build && cd build
cmake ..
cmake --build .
```

---

## Usage

Place a CHIP-8 ROM file in the project directory or provide a full path, then update the ROM path in `main.cpp`:

```cpp
chip8::load_rom(chip8_0, "path/to/your/rom.ch8");
```

Run the compiled binary. Press `ESC` to quit.

---

## Keypad Mapping

The original CHIP-8 hexadecimal keypad is mapped to the following keyboard layout:

```
CHIP-8 Key    Keyboard
──────────    ────────
1 2 3 C   →  1 2 3 4
4 5 6 D   →  Q W E R
7 8 9 E   →  A S D F
A 0 B F   →  Z X C V
```

---

## Architecture

The emulator is structured around a single `chip8` class defined in `chip8.h` containing:

| Component | Details |
|-----------|---------|
| RAM | 4096 bytes, programs load at `0x200` |
| Registers | 16 general-purpose 8-bit registers (`V0`–`VF`) |
| Index Register | 16-bit register `I` for memory addressing |
| Program Counter | 16-bit `PC`, starts at `0x200` |
| Stack | 16-level return address stack |
| Timers | 8-bit delay and sound timers, decrement at 60 Hz |
| Display | 64×32 monochrome pixel buffer |
| Keypad | 16-key state array |
| Font | Built-in hex digit sprites loaded at `0x050` |

---

## Opcode Coverage

| Group | Opcodes |
|-------|---------|
| `0x0` | `00E0` (CLS), `00EE` (RET) |
| `0x1` | `1NNN` (JP) |
| `0x2` | `2NNN` (CALL) |
| `0x3` | `3XNN` (SE VX, NN) |
| `0x4` | `4XNN` (SNE VX, NN) |
| `0x5` | `5XY0` (SE VX, VY) |
| `0x6` | `6XNN` (LD VX, NN) |
| `0x7` | `7XNN` (ADD VX, NN) |
| `0x8` | `8XY0`–`8XY7`, `8XYE` (ALU ops) |
| `0x9` | `9XY0` (SNE VX, VY) |
| `0xA` | `ANNN` (LD I, NNN) |
| `0xB` | `BNNN` (JP V0, NNN) |
| `0xC` | `CXNN` (RND VX, NN) |
| `0xD` | `DXYN` (DRW VX, VY, N) |
| `0xE` | `EX9E` (SKP VX), `EXA1` (SKNP VX) |
| `0xF` | `FX07`, `FX0A`, `FX15`, `FX18`, `FX1E`, `FX29`, `FX33`, `FX55`, `FX65` |

---

## Quirks

This emulator targets original CHIP-8 behavior:

| Quirk | Behavior |
|-------|----------|
| VF Reset | VF is reset to 0 after `8XY1`, `8XY2`, `8XY3` |
| Memory | `FX55`/`FX65` increment the I register |
| Shifting | `8XY6`/`8XYE` shift VY into VX (original behavior) |
| Clipping | Sprites that go off screen edges are clipped, not wrapped |
| Jumping | `BNNN` uses `V0` as offset |

---

## Testing

ROMs used for testing, in recommended order:

1. `2-ibm-logo.ch8` — basic display sanity check
2. `3-corax+.ch8` — arithmetic opcode verification
3. `4-flags.ch8` — VF flag behavior
4. `5-quirks.ch8` — quirks compatibility
5. `6-keypad.ch8` — input handling

Test ROMs are from [Timendus's CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite).

Game ROMs can be found at [kripod/chip8-roms](https://github.com/kripod/chip8-roms).

---

## Project Structure

```
CHIP-8/
├── chip8.h       ← CPU, memory, opcodes, init, ROM loader
├── main.cpp      ← SDL3 setup, main loop, render, input, audio
├── CMakeLists.txt
├── roms/         ← Place ROM files here
└── README.md
```

---

## Known Limitations

- No SUPER-CHIP (SCHIP) or XO-CHIP support
- Single emulator instance per window
- No save states
- No ROM selection UI — ROM path is hardcoded in `main.cpp`

---

## License

MIT License. Free to use, modify, and distribute.
