#include "global.h"
#include "chip8.h"
#include <string>
#include <SDL2/SDL.h>
#include <unistd.h>

#include <iomanip>

uint8_t keymap[16] = {
    SDLK_x, SDLK_1, SDLK_4, SDLK_3, SDLK_q, SDLK_r, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_2, SDLK_w, SDLK_f, SDLK_v,
};

uint8_t reset_key = SDLK_SPACE;

uint8_t SCREEN_SCALE = 1;

using namespace std;

int main(int argc, char *argv[]) {
    // Bools for debug/walk modes
    bool debug = false;
    bool walk = false;

    // Reset bool
    bool reset = false;

    // Look for ROM path argument
    if (argc <= 1) {
        std::cerr << "Provide ROM file as argument\n";
        exit(1);
    }
    if (strcmp(argv[1], "-help") == 0) {
        // Help Menu
        std::cout
            << "Play a game: ./chip8 <path_to_rom>\n"
            << "Optional flags can be given after rom path:\n"
            << "-s    sets screen scale in multiples of 2, default is 1 (640x320)"
            << "-w    walk mode/single step. Press enter at cli to walk and Esc then enter to quit\n"
            << "-t    trace mode - Prints trace information about program counter, registers, opcodes, "
               "etc"
            << "<pc> <opcode> <I> <sp> <V0 to VF registers>\n";
        exit(0);
    }
    // Look for additional flags
    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-d") == 0) {
                debug = true;
            } else if (strcmp(argv[i], "-w") == 0) {
                walk = true;
            } else if (strcmp(argv[i], "-s") == 0) {
                SCREEN_SCALE = atoi(argv[i + 1]);
                cout << "SCALE " << atoi(argv[i + 1]);
                break;
            } else {
                std::cerr << "Invalid flags -  Type -help to check usage\n";
                exit(1);
            }
        }
    }

    // Create SDL structures
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    // Init SDL and check for errors
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Error in initialising SDL " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    while (true) {
        // Construct CPU instance
        Cpu cpu;

        // Check for errors loading ROM
        if (!cpu.loadRom(argv[1])) // loading ROM provided as argument
        {
            cerr << "Error loading ROM, possibly invalid.\n";
            exit(1);
        }

        // Create Window/Renderer
        SDL_CreateWindowAndRenderer(globals::SCREEN_WIDTH * SCREEN_SCALE,
                                    globals::SCREEN_HEIGHT * SCREEN_SCALE, 0, &window, &renderer);

        // Check for errors creating window
        if (window == nullptr) {
            std::cerr << "Error in creating window " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }

        // Check for errors creating renderer
        if (renderer == nullptr) {
            std::cerr << "Error in initializing rendering " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }

        // SDL_RenderSetLogicalSize(renderer, globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT);

        // Create SDL texture
        texture =
            SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

        // Check for errors creating texture
        if (texture == nullptr) {
            std::cerr << "Error in setting up texture " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }

        // Inner Game Loop
        while (!reset) {
            // Word opcode = cpu.fetchOpcode();

            // Run CPU Cycle
            cpu.executeOpcode(debug);

            // Create SDL Event structure
            SDL_Event event;

            // Poll for SDL Events
            while (SDL_PollEvent(&event)) {

                // Check for QUIT/CLOSE
                if (event.type == SDL_QUIT) {
                    exit(0);
                }

                // Check for key press
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        exit(0);
                    }
                    if (event.key.keysym.sym == reset_key) {
                        reset = true;
                    }
                    // Map key to chip8 input
                    for (int i = 0; i < 16; ++i) {
                        if (event.key.keysym.sym == keymap[i]) {
                            cpu.input[i] = 1;
                        }
                    }
                }

                // Check for key release
                if (event.type == SDL_KEYUP) {

                    // Map key to chip8 input
                    for (int i = 0; i < 16; ++i) {
                        if (event.key.keysym.sym == keymap[i]) {
                            cpu.input[i] = 0;
                        }
                    }
                }
            }
            // Check draw flag - draw on screen if true
            if (cpu.draw) {
                cpu.draw = false;
                uint32_t pixels[32 * 64];
                for (int i = 0; i < 32 * 64; i++) {
                    if (cpu.display[i] == 0) {
                        pixels[i] = 0xFF000000;
                    } else {
                        pixels[i] = 0xFFFFFFFF;
                    }
                }

                // SDL Drawing
                SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }

            // Imitate CHIP8 speed using sleep
            usleep(1500);

            // Check for walk mode flag and walk if true
            if (walk) {
                std::string temp;
                getline(std::cin, temp);
                if (temp[0] == 27) { // ESC key
                    exit(0);
                }
            }
            // if (reset) {
            //     cout << "Resetting Chip8" << endl;
            //     cpu.~Cpu();
            //     break;
            // }
        }
        // Check for reset to return to top of loop
        if (reset) {
            cout << "Resetting Chip8" << endl;
            // For safety, call CPU deconstructor to ensure it gets freshly constructed
            cpu.~Cpu();
            // set reset back to false to prevent endless reset loop
            reset = false;
            // jump back to top of main loop
            continue;
        }
    }
}