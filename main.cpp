#include <cstdint>
#include <iostream>
#include <SDL3/SDL.h>
#include "chip8.h"

using namespace std;

// For Drawing
static SDL_Window*   window   = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture*  texture  = nullptr;

// For Sound
static SDL_AudioStream* audio_stream = nullptr;

constexpr int SCALE = 10;  // 64*10=640, 32*10=320


chip8 chip8_0;
chip8 chip8_1;

void render(chip8 chip);
void get_Input(SDL_Event* e, chip8 &chip);
void generate_beep(SDL_AudioStream* stream);
void run(chip8 &chip, const char* rOm);

int main(){
    const char* rOM = "C:/Users/USER/CLionProjects/CHIP-8/roms/games/Airplane.ch8";
    // const char* rOOM = "C:/Users/USER/CLionProjects/CHIP-8/roms/4-flags.ch8";
    std::cout << sizeof(chip8_0.memory) << std::endl;



    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    window   = SDL_CreateWindow("CHIP-8", 64 * SCALE, 32 * SCALE, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    texture  = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    SDL_AudioSpec spec;
    spec.freq     = 44100;
    spec.format   = SDL_AUDIO_S16;
    spec.channels = 1;

    audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    SDL_ResumeAudioStreamDevice(audio_stream);

    // For running multiple chip8 systems
    // ................. NOTE THAT EACH INSTANCE DOES NOT RUN SIMULTANEOUSLY BUT ONE AFTER ANOTHER .................................
    // First instance
    run(chip8_0, rOM);

    // Second instance
    // run(chip8_1, rOOM);

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


void render(chip8 chip) {
    // Build RGBA pixel buffer from display array
    uint32_t pixels[64 * 32];
    for (int i = 0; i < 64 * 32; i++) {
        pixels[i] = chip.display[i] ? 0xFFFFFFFF : 0x000000FF;
    }

    // Push pixels to texture
    SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);  // stretches to window size
    SDL_RenderPresent(renderer);

    chip.draw_flag = false;  // reset flag after drawing
}

void get_Input(SDL_Event* e, chip8 &chip) {
    bool pressed = (e->type == SDL_EVENT_KEY_DOWN);
    switch (e->key.scancode) {
        case SDL_SCANCODE_1: chip.keypad[0x1] = pressed; break;
        case SDL_SCANCODE_2: chip.keypad[0x2] = pressed; break;
        case SDL_SCANCODE_3: chip.keypad[0x3] = pressed; break;
        case SDL_SCANCODE_4: chip.keypad[0xC] = pressed; break;
        case SDL_SCANCODE_Q: chip.keypad[0x4] = pressed; break;
        case SDL_SCANCODE_W: chip.keypad[0x5] = pressed; break;
        case SDL_SCANCODE_E: chip.keypad[0x6] = pressed; break;
        case SDL_SCANCODE_R: chip.keypad[0xD] = pressed; break;
        case SDL_SCANCODE_A: chip.keypad[0x7] = pressed; break;
        case SDL_SCANCODE_S: chip.keypad[0x8] = pressed; break;
        case SDL_SCANCODE_D: chip.keypad[0x9] = pressed; break;
        case SDL_SCANCODE_F: chip.keypad[0xE] = pressed; break;
        case SDL_SCANCODE_Z: chip.keypad[0xA] = pressed; break;
        case SDL_SCANCODE_X: chip.keypad[0x0] = pressed; break;
        case SDL_SCANCODE_C: chip.keypad[0xB] = pressed; break;
        case SDL_SCANCODE_V: chip.keypad[0xF] = pressed; break;
        case SDL_SCANCODE_ESCAPE: chip.running = false; break;
        default: ;
    }
}

void generate_beep(SDL_AudioStream* stream) {
    // Generate a simple square wave
    constexpr int sample_rate = 44100;
    constexpr int duration_ms = 16;     // one frame worth of audio
    constexpr int num_samples = (sample_rate * duration_ms) / 1000;

    int16_t samples[num_samples];
    for (int i = 0; i < num_samples; i++) {
        const int frequency   = 440;
        // Square wave — alternates between high and low
        samples[i] = (i / (sample_rate / frequency / 2) % 2 == 0) ? 8000 : -8000;
    }

    SDL_PutAudioStreamData(stream, samples, num_samples * sizeof(int16_t));
}

void run(chip8 &chip, const char* rOm) {
    // Initialize Chip-8 system
    chip8::initialize(chip);

    // Load game file
    chip8::load_rom( chip, rOm);

    while (chip.running) {
        // User defined cycle rate per frame rate
        for (int i = 0; i < 10; i++ )
            chip8::run_Cycle(chip);

        // Update timers
        if (chip.delay_timer > 0)
            --chip.delay_timer;

        if (chip.sound_timer > 0) {
            generate_beep(audio_stream);  // feed audio every frame timer is active
            --chip.sound_timer;
        }

        // Update the screen when due
        if (chip.draw_flag)
            render(chip);

        // Get key presses and releases
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) chip8_0.running = false;
            get_Input(&e, chip);
        }

        SDL_DelayPrecise(16666667);
    }
}