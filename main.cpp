#include <cstdint>
#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include "chip8.h"

using namespace std;

// static SDL_Window *wndow = NULL;
// static SDL_Window *renderer = NULL;


chip8 chip8_0;

void render();
void get_Input();

int main(){
    std::cout << sizeof(chip8_0.memory) << std::endl;

    // Initialize Chip-8 system
    chip8::initialize(chip8_0);

    // Load game file
    chip8_0.load_rom( chip8_0, "Hello.whatever");

    // while (chip8_0.running) {
    //     // User defined cycle rate per frame rate
    //     for (int i = 0; i < 10; i++ )
    //         chip8_0.run_Cycle(chip8_0);
    //
    //     // Update timers
    //     if(chip8_0.delay_timer > 0)
    //         --chip8_0.delay_timer;
    //
    //     if(chip8_0.sound_timer > 0) {
    //         if(chip8_0.sound_timer == 1)
    //             // Play sound
    //                 printf("BEEP!\n");
    //         --chip8_0.sound_timer;
    //     }
    //
    //     // Update the screen when due
    //     if (chip8_0.draw_flag)
            render();

        // Get key presses and releases
    //     get_Input();
    // }

    return 0;
}


void render() {
    int a = 5;
    int b = 9;
    if ((a += b) > 8)
        cout << a << endl;
}

void get_Input() {
    // Get user input
}