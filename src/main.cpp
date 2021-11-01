#include "global.h"
#include "chip8.h"
#include <string>
#include <SDL2/SDL.h>
#include <unistd.h>

#include <iomanip>

uint8_t keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_4,
        SDLK_3,
        SDLK_q,
        SDLK_r,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_2,
        SDLK_w,
        SDLK_f,
        SDLK_v,
};



using namespace std;

int main(int argc, char *argv[]) {
    bool debug = false;
    bool walk = false;


    if (argc <=1) {
        std::cerr << "Provide ROM file as argument\n";
        exit(1);
    }
    if (strcmp(argv[1], "-help") == 0)
    {
        //print the help menu
        std::cout << "Play a game: ./chip8 <path_to_rom>\n"
                  << "Optional flags can be given after rom path:\n"
                  << "-w    walk mode/single step. Press enter at cli to walk and Esc then enter to quit\n"
                  << "-t    trace mode - Prints trace information about program counter, registers, opcodes, etc"
                  << "<pc> <opcode> <I> <sp> <V0 to VF registers>\n";
        exit(0);
    }
    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-d") == 0) {
                debug = true;
            } else if (strcmp(argv[i], "-w") == 0) {
                walk = true;
            } else {
                std::cerr << "Invalid flags given. Type -help to check usage\n";
                exit(1);
            }
        }
    }
    Cpu cpu;
    cpu.loadFonts();


    cout << showbase // show the 0x prefix
         << internal // fill between the prefix and the number
         << setfill('0'); // fill with 0s

    cout << "PC: " << hex << cpu.pc << endl;

    if (!cpu.loadRom(argv[1])) //loading ROM provided as argument
    {
        cerr << "ROM could not be loaded.\n";
        exit(1);
    }

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

     if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "Error in initialising SDL " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    SDL_CreateWindowAndRenderer(globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT, 0, &window, &renderer);
    // window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320,
                            //   SDL_WINDOW_SHOWN);

    if (window == nullptr)
    {
        std::cerr << "Error in creating window " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr)
    {
        std::cerr << "Error in initializing rendering " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // SDL_RenderSetLogicalSize(renderer, globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (texture == nullptr)
    {
        std::cerr << "Error in setting up texture " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    while (true) {
        // Word opcode = cpu.fetchOpcode();
        cpu.executeOpcode(debug);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    exit(0);
                }

                for (int i = 0; i < 16; ++i)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        cpu.input[i] = 1;
                    }
                }
            }

            if (event.type == SDL_KEYUP)
            {
                for (int i = 0; i < 16; ++i)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        cpu.input[i] = 0;
                    }
                }
            }

        }

    if (cpu.draw) {
        cpu.draw = false;
        uint32_t pixels[32 * 64];
        for (int i = 0; i < 32 * 64; i++)
            {
                if (cpu.display[i] == 0)
                {
                    pixels[i] = 0xFF000000;
                }
                else
                {
                    pixels[i] = 0xFFFFFFFF;
                }
            }

            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
    }
    usleep(1500);
    if (walk)
        {
            std::string temp;
            getline(std::cin, temp);
            if (temp[0] == 27) //esc
            {
                exit(0);
            }
        }
    }

}