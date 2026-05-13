//
// Created by USER on 04/05/2026.
//

#ifndef CHIP_8_CHIP8_H
#define CHIP_8_CHIP8_H
#include <cstdint>
#include <cstdlib>
#include <string>
#include <random>
#include <fstream>
using namespace std;

class chip8 {
public:
    static void initialize(chip8 &chip, int pc = 0x200, int op = 0, int in = 0, int s = 0);

    static void load_rom(chip8 &chip, const char *rom);

    static void run_Cycle(chip8 &chip);


    uint8_t memory[4096]{}; // RAM
    uint8_t V[16]{}; // registers
    uint16_t PC{}; // program counter
    uint16_t I{};  // Index register
    uint16_t stack[16]{}; // stack
    uint8_t SP{}; // stack pointer
    uint8_t delay_timer{}; // delay timer
    uint8_t sound_timer{}; // sound timer
    uint8_t display[64*32]{}; // Display size
    uint8_t keypad[16]{};
    uint16_t opcode{};
    bool draw_flag{};
    int8_t key_wait{};

    static constexpr int fontset_size = 80;

    // Font set
    uint8_t fontset[fontset_size] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    bool running = true;
};

// Initialize the system. Can be customized by the user based on the specs wanted
inline void chip8::initialize(chip8 &chip, int pc, int op, int in, int s) {
    chip.PC = pc; // Program counter starts at 0x200
    chip.I = in;
    chip.SP = s;
    chip.opcode = op;
    chip.key_wait = -1;
    chip.draw_flag = false;
    chip.delay_timer = 0;
    chip.sound_timer = 0;
    memset(chip.stack,   0, sizeof(chip.stack));
    memset(chip.V,       0, sizeof(chip.V));
    memset(chip.keypad,  0, sizeof(chip.keypad));

    // Initialize memory and display
    memset(chip.memory, 0, sizeof(chip.memory));
    memset(chip.display, 0, sizeof(chip.display));

    // Load fontset
    for (int i = 0; i < chip8::fontset_size; i++) {
        chip.memory[0x050 + i] = chip.fontset[i]; // Loaded at memory address 0x50
    }

    srand(time(NULL)); // For random number generation

    std::cout << "System Initialized!!" << endl;
}

inline void chip8::load_rom(chip8 &chip, const char* rom) {
    // Open chip-8 game file as binary
    ifstream rom_file(rom, std::ios::binary | std::ios::ate);

    if (rom_file.is_open()) {
        // Get the size of the file
        std::streampos file_size = rom_file.tellg();
        char* buffer = new char[file_size];

        rom_file.seekg(0, std::ios::beg);
        rom_file.read(buffer, file_size);
        rom_file.close();

        // Load ROM contents into memory at 0x200
        for (long i = 0; i < file_size; i++) {
            chip.memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
        }

        delete[] buffer;
    }else {
        fprintf(stderr, "Failed to open rom file %s\n", rom);
    }
}

inline void chip8::run_Cycle(chip8 &chip) {
    // Fetch opcode
    chip.opcode = chip.memory[chip.PC] << 8 | chip.memory[chip.PC + 1];
    chip.PC += 2;

    // Decode and Execute opcode
    switch (chip.opcode >> 0xC) {
        case 0x0:
            switch (chip.opcode & 0x00FF) {
                case 0xE0:
                    memset(chip.display, 0, sizeof(chip.display));
                    break;
                case 0xEE:
                    chip.PC = chip.stack[--chip.SP];
                    break;
                default:
                    fprintf(stderr, "UNKNOWN OPCODE: 0x%04X at PC=0x%03X\n", chip.opcode, chip.PC - 2);
                    break;
            }
            break;
        case 0x1:
            // Jump to address
            chip.PC = chip.opcode & 0x0FFF;
            break;
        case 0x2:
            // Jump to subroutine while saving previous routine in stack
            chip.stack[chip.SP++] = chip.PC;
            chip.PC = chip.opcode & 0x0FFF;
            break;
        case 0x3:
            if (chip.V[(chip.opcode & 0x0F00) >> 8] == (chip.opcode & 0x00FF)) {
                chip.PC += 2 ;
            }
            break;
        case 0x4:
            if (chip.V[(chip.opcode & 0x0F00) >> 8] != (chip.opcode & 0x00FF)) {
                chip.PC += 2 ;
            }
            break;
        case 0x5:
            if (chip.V[(chip.opcode & 0x0F00) >> 8] == chip.V[(chip.opcode & 0x00F0) >> 4]) {
                chip.PC += 2 ;
            }
            break;
        case 0x6:
            chip.V[(chip.opcode & 0x0F00) >> 8] = chip.opcode & 0x00FF;
            break;
        case 0x7:
            chip.V[(chip.opcode & 0x0F00) >> 8] += chip.opcode & 0x00FF;
            break;
        case 0x8:
            switch (chip.opcode & 0x000F) {
                case 0x0:
                    chip.V[(chip.opcode & 0x0F00) >> 8] = chip.V[(chip.opcode & 0x00F0) >> 4];
                    break;
                case 0x1:
                    chip.V[(chip.opcode & 0x0F00) >> 8] |= chip.V[(chip.opcode & 0x00F0) >> 4];
                    chip.V[0xF] = 0;
                    break;
                case 0x2:
                    chip.V[(chip.opcode & 0x0F00) >> 8] &= chip.V[(chip.opcode & 0x00F0) >> 4];
                    chip.V[0xF] = 0;
                    break;
                case 0x3:
                    chip.V[(chip.opcode & 0x0F00) >> 8] ^= chip.V[(chip.opcode & 0x00F0) >> 4];
                    chip.V[0xF] = 0;
                    break;
                case 0x4:
                    chip.V[0xF] = ((chip.V[(chip.opcode & 0x0F00) >> 8] + chip.V[(chip.opcode & 0x00F0) >> 4]) > 255) ? 1 : 0;
                    chip.V[(chip.opcode & 0x0F00) >> 8] = (chip.V[(chip.opcode & 0x0F00) >> 8] + chip.V[(chip.opcode & 0x00F0) >> 4]) & 0xFF ;
                    break;
                case 0x5:
                    chip.V[0xF] = ((chip.V[(chip.opcode & 0x0F00) >> 8] >= chip.V[(chip.opcode & 0x00F0) >> 4])) ? 1 : 0;
                    chip.V[(chip.opcode & 0x0F00) >> 8] -= chip.V[(chip.opcode & 0x00F0) >> 4];
                    break;
                case 0x6:
                    chip.V[0xF] = (chip.V[(chip.opcode & 0x00F0) >> 4] & 0x1); // Get the least significant bit
                    chip.V[(chip.opcode & 0x0F00) >> 8] = (chip.V[(chip.opcode & 0x00F0) >> 4]) >> 1;
                    break;
                case 0x7:
                    chip.V[0XF] = (chip.V[(chip.opcode & 0x0F00) >> 8] > chip.V[(chip.opcode & 0x00F0) >> 4]) ? 0 : 1 ;
                    chip.V[(chip.opcode & 0x0F00) >> 8] = chip.V[(chip.opcode & 0x00F0) >> 4] - chip.V[(chip.opcode & 0x0F00) >> 8];
                    break;
                case 0xE:
                    chip.V[0xF] = chip.V[(chip.opcode & 0x00F0) >> 4] >> 7; // Get most significant bit
                    chip.V[(chip.opcode & 0x0F00) >> 8] = chip.V[(chip.opcode & 0x00F0) >> 4] << 1;
                    break;
                default:
                    fprintf(stderr, "UNKNOWN OPCODE: 0x%04X at PC=0x%03X\n", chip.opcode, chip.PC - 2);
                    break;
            }
            break;
        case 0x9:
            if (chip.V[(chip.opcode & 0x0F00) >> 8] != chip.V[(chip.opcode & 0x00F0) >> 4]) {
                chip.PC += 2;
            }
            break;
        case 0xA:
            chip.I = chip.opcode & 0x0FFF;
            break;
        case 0xB:
            chip.PC = (chip.opcode & 0x0FFF) + chip.V[0x0];
            break;
        case 0xC:
            chip.V[(chip.opcode & 0x0F00) >> 8] = (rand() % 256) & (chip.opcode & 0x00FF);
            break;

        case 0xD: { // Handle sprite drawing
            chip.draw_flag = true;
            uint8_t x = chip.V[(chip.opcode & 0x0F00) >> 8];
            uint8_t y = chip.V[(chip.opcode & 0x00F0) >> 4];
            uint8_t n = chip.opcode & 0x000F;

            // Clear collision flag
            chip.V[0xF] = 0;

            for (int row = 0; row < n; row++) {
                uint8_t sprite_byte = chip.memory[chip.I + row];
                for (int col = 0; col < 8; col++) {
                    if (sprite_byte & (0x80 >> col)) {
                        int px = x + col;   // raw, no modulo
                        int py = y + row;   // raw, no modulo
                        if (px >= 64 || py >= 32) continue;  // now this actually works
                        int idx = py * 64 + px;
                        if (chip.display[idx] == 1) chip.V[0xF] = 1;
                        chip.display[idx] ^= 1;
                    }
                }
            }
            break;
        }
        case 0xE:
            switch (chip.opcode & 0x00FF) {
            case 0x9E:
                if (chip.keypad[chip.V[(chip.opcode & 0x0F00) >> 8]])
                    chip.PC += 2;
                break;
            case 0xA1:
                if (!chip.keypad[chip.V[(chip.opcode & 0x0F00) >> 8]])
                    chip.PC += 2;
                break;
            default:
                fprintf(stderr, "UNKNOWN OPCODE: 0x%04X at PC=0x%03X\n", chip.opcode, chip.PC - 2);
                break;
            }
            break;
        case 0xF:
            switch (chip.opcode & 0x00FF) {
                case 0x07:
                    chip.V[(chip.opcode & 0x0F00) >> 8] = chip.delay_timer;
                    break;
            case 0x0A: {
                if (chip.key_wait == -1) {
                    // Phase 1: wait for any key press
                    bool found = false;
                    for (int i = 0; i < 16; i++) {
                        if (chip.keypad[i]) {
                            chip.key_wait = i;  // remember which key was pressed
                            found = true;
                            break;
                        }
                    }
                    if (!found) chip.PC -= 2;  // no key yet, keep waiting
                } else {
                    // Phase 2: wait for that key to be released
                    if (!chip.keypad[chip.key_wait]) {
                        chip.V[(chip.opcode & 0x0F00) >> 8] = chip.key_wait;
                        chip.key_wait = -1;  // done
                    } else {
                        chip.PC -= 2;  // still held, keep waiting
                    }
                }
                break;
            }
                case 0x15:
                    chip.delay_timer = chip.V[(chip.opcode & 0x0F00) >> 8];
                    break;
                case 0x18:
                    chip.sound_timer = chip.V[(chip.opcode & 0x0F00) >> 8];
                    break;
                case 0x1E:
                    chip.I += chip.V[(chip.opcode & 0x0F00) >> 8];
                    break;
                case 0x29:
                    chip.I = 0x050 + (chip.V[(chip.opcode & 0x0F00) >> 8] * 5);
                    break;
                case 0x33: {
                    uint8_t val = chip.V[(chip.opcode & 0x0F00) >> 8];
                    chip.memory[chip.I]     = val / 100;        // hundreds
                    chip.memory[chip.I + 1] = (val / 10) % 10;  // tens
                    chip.memory[chip.I + 2] = val % 10;          // ones
                    break;
                }
                case 0x55:
                    for (int i = 0; i <= ((chip.opcode & 0x0F00) >> 8); i++) {
                        chip.memory[chip.I + i] = chip.V[i];
                    }
                    chip.I = chip.I + ((chip.opcode & 0x0F00) >> 8) + 1;
                    break;
                case 0x65:
                    for (int i = 0; i <= ((chip.opcode & 0x0F00) >> 8); i++) {
                        chip.V[i] = chip.memory[chip.I + i];
                    }
                    chip.I = chip.I + ((chip.opcode & 0x0F00) >> 8) + 1;
                    break;


                default:
                    fprintf(stderr, "UNKNOWN OPCODE: 0x%04X at PC=0x%03X\n", chip.opcode, chip.PC - 2);
                    break;
            }
            break;


        default:
            fprintf(stderr, "UNKNOWN OPCODE: 0x%04X at PC=0x%03X\n", chip.opcode, chip.PC - 2);
            break;
    }
}


#endif //CHIP_8_CHIP8_H