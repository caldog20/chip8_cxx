#include "global.h"
#include "chip8.h"
#include <fstream>
#include <cstring>
#include <string>

using namespace std;

Cpu::Cpu() {
    // Constructor for CPU = Default Settings
    pc = 0x200;
    i = 0;
    sp = 0;
    st = 0;
    dt = 0;
    draw = false;
    counter = 10;

    // Zero out registers, stack, and ram on construction
    memset(v, 0, sizeof(v));
    memset(stack, 0, sizeof(stack));
    memset(ram, 0, sizeof(ram));
    memset(display, 0, sizeof(display));
    memset(input, 0, sizeof(input));
}

// Read ROM file into ram starting at 512 bytes in - return false if error occurs
bool Cpu::loadRom(std::string path) {
    std::ifstream input_rom(path, std::ios::binary | std::ios::in);
    if (!input_rom.is_open()) {
        return false;
        std::perror("ROM file is being used by something else");
    }
    char c;
    int j = 512;
    for (int i = 0x200; input_rom.get(c); i++) {
        if (j >= 4096) {
            return false;
            std::perror("ROM file too big");
        }
        ram[i] = (Byte) c;
        j++;
    }
    return true;
}

void Cpu::loadFonts() {
    unsigned char fonts[80] =
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

    for (int t = 0; t < 80; t++) {
        ram[t] = fonts[t];
    }
}

// Word Cpu::fetchOpcode() {
//     Word opcode = (ram[pc] << 8) | (ram[pc + 1]);
//     std::cout << "opcode- " << hex << opcode << endl;
//     return opcode;
// }

void Cpu::executeOpcode(bool debug) {
    /*
        Program Counter = pc
        Stack Pointer = sp
        CPU Registers = v
        Stack = stack
        Index Register = i
        ms_byte = most significant byte
        nnn = lower 12 bits of instruction
        kk = lowest 8 bits of instruction
        nx = lower 4 bits of high byte
        ny = lower 4 bits of low byte
        n = lower 4 bits if instruction
        */

    Word opcode = (ram[pc] << 8) | (ram[pc + 1]);

    if (debug)
    {
        printf("pc: %.4X opcode: %.4X sp: %.2X ", pc, opcode, sp);
        for (int i = 0; i < 15; i++) {
            printf("v[$d]:%.2X ", v[i]);
        }
    }

    Word ms_byte = (opcode & 0xF000) >> 12;

    int nibbles[4] = {(opcode & 0xF000), ((opcode & 0x0F00) >> 8), ((opcode & 0x00F0) >> 4), (opcode & 0x000F)};

    Word nnn = opcode & 0x0FFF;
    
    Byte kk = opcode & 0x00FF;
    Byte nx = nibbles[1];
    Byte ny = nibbles[2];
    Byte n = nibbles[3];

    switch(ms_byte) {
        // 0x0 Sub instructions
        case 0x0:
            switch(opcode) {
                // CLEAR SCREEN
                case 0x00E0:
                    // cout << "CLEAR SCREEN" << endl;
                    memset(display, 0, sizeof(display));
                    draw = true;
                    pc += 2;
                    break;
                // RETURN SUBROUTINE    
                case 0x00EE:
                    // cout << "RETURN SUB" << endl;
                    sp--;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:
                    break;
            }
            break;
        // JUMP ADDR
        case 1:
            // std::cout << "JUMP ADDR" << endl;
            pc = nnn;
            break;
        
        // CALL ADDR
        case 2:
            // cout << "CALL ADDR" << endl;
            stack[sp] = pc;
            sp++;
            pc = nnn;
            break;

        // SKIP E VX KK
        case 0x3:{
            // cout << "SKIP E VX KK" << endl;
            pc += 2;
            if (v[nx] == kk) {
                pc += 2;
            }
            break;
        }
        // SKIP NE VX KK
        case 0x4: {
            // cout << "SKIP NE VX KK" << endl;
            pc += 2;
            if (v[nx] != kk) {
                pc += 2;
            }
            break;
        }
        // SKIP E VX VY
        case 0x5:
            // cout << "SKIP E VX VY" << endl;
            pc += 2;
            if (v[nx] == v[ny]) {
                pc += 2;
            }
            break;

        // LOAD VX KK
        case 0x6:
            // cout << "LOAD VX KK" << endl;
            // printf("KK VAL: %.4X\n", kk);
            v[nx] = kk;
            pc += 2;
            break;

        // ADD VX KK
        case 0x7: 
            // cout << "ADD VX KK" << endl;
            v[nx] += kk;
            pc += 2;
            break;
        case 0x8:
            // 0x8 Sub Instructions
            switch(n) {
                // load_vx_vy
                case 0x0:
                    // cout << "LOAD VX VY" << endl;
                    v[nx] = v[ny];
                    pc += 2;
                    break;
                // or vx vy
                case 0x1:
                    // cout << "OR VX VY" << endl;
                    v[nx] |= v[ny];
                    v[0xF] = 0;
                    pc += 2;
                    break;
                // and vx vy
                case 0x2:
                    // cout << "AND VX VY" << endl;
                    v[nx] &= v[ny];
                    v[0xF] = 0;
                    pc += 2;
                    break;
                // xor vx vy
                case 0x3:
                    // cout << "XOR VX VY" << endl;
                    v[nx] ^= v[ny];
                    v[0xF] = 0;
                    pc += 2;
                    break;

                //add vx vy
                case 0x4:
                    // cout << "ADD VX VY" << endl;
                    if ((v[nx] + v[ny]) > 0xFF) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[nx] += v[ny];
                    v[nx] = (int8_t) v[nx];
                    pc += 2;
                    break;

                //sub vx vy
                case 0x5:
                    // cout << "SUB VX VY" << endl;
                    if (v[nx] < v[ny]) {
                        v[0xF] = 0;
                    } else {
                        v[0xF] = 1;
                    }
                    v[nx] = (uint8_t) v[nx] - (uint8_t) v[ny];
                    pc += 2;
                    break;
                
                //shr vx
                case 0x6:
                    cout << "SHR VX" << endl;
                    v[0xF] = v[nx] & 0x01;
                    v[nx] >>= 1;
                    v[nx] = (uint8_t) v[nx];
                    pc += 2;
                    break;

                // subn vx vy
                case 0x7:
                    // cout << "SUBN VX VY" << endl;
                    if (v[nx] > v[ny]) {
                        v[0xF] = 0;
                    } else {
                        v[0xF] = 1;
                    }
                    v[nx] = (uint8_t) v[ny] - (uint8_t) v[nx];
                    pc += 2;
                    break;
                
                // shl vx
                case 0xE:
                    cout << "SHL VX" << endl;
                    v[0xF] = v[nx] >> 7;
                    v[0xF] = (uint8_t) v[0xF];
                    v[nx] <<= 1;
                    v[nx] = (uint8_t) v[nx];
                    pc += 2;
                    break;
                
                // default catchall
                default:
                    std::cerr << "Unknown opcode- " << hex << opcode << endl;
                    break;

            }
            break;
        // SKIP NE VX VY
        case 0x9:
            // cout << "SKIP NE VX VY" << endl;
            pc += 2;
            if (v[nx] != v[ny]) {
                pc += 2;
            }
            break;
        // LOAD I ADDR
        case 0xA:
            // cout << "LOAD I ADDR" << endl;
            i = nnn;
            pc += 2;
            break;
        // JUMP V0 ADDR
        case 0xB:
            // cout << "JUMP V0 ADDR" << endl;
            pc = nnn;
            pc += v[0];
            break;
        // RAN VX KK
        case 0xC:
            // cout << "RAN VX KK" << endl;
            v[nx] = 5 & kk;
            pc += 2;
            break;
        case 0xD: {
            // cout << "DRAW" << endl;
            int x = v[nx], y = v[ny];
            int ht = opcode & 0x000F; //N
            int wt = 8;
            v[0xF] = 0;

            for (int j = 0; j < ht; j++)
            {
                int pixel = ram[i + j];
                for (int k = 0; k < wt; k++)
                {
                    if ((pixel & (0x80 >> k)) != 0)
                    {
                        int index = ((x + k) + ((y + j) * 64)) % 2048;
                        if (display[index] == 1)
                        {
                            v[0x0F] = 1;
                        }
                        display[index] ^= 1;
                    }
                }
            }

            draw = true;
            pc += 2;
            break;
        }

        // multiple Sub Instructions
        case 14:
            switch(kk) {
                // SKIP P VX
                case 0x9E:
                    // Word key = v[nx];
                    pc += 2;
                    if (input[v[nx]] != 0) {
                        pc += 2;
                    }
                    break;
                // SKIP NP VX
                case 0xA1:
                    // Word key = v[nx];
                    pc += 2;
                    if (input[v[nx]] == 0) {
                        pc += 2;
                    } 
                    break;
                // default catchall
                default:
                    std::cerr << "0xE Unknown opcode- " << hex << opcode << endl;
                    break;
            }
            break;

        case 0xF:
            switch (kk) {
                //LOAD VX DT
                case 0x07:
                    v[nx] = dt;
                    pc += 2;
                    break;
                // LOAD VX P
                case 0x0A:
                {
                    bool key_pressed = false;
                    for (int t = 0; t < 16; t++) {
                        if (input[t] == true) {
                            key_pressed = true;
                            v[nx] = (uint8_t) t;
                        }
                    }
                    if (key_pressed) {
                        pc += 2;
                    }
                    break;
                }
                // LOAD DT VX
                case 0x15:
                    dt = v[nx];
                    pc += 2;
                    break;
                // LOAD ST VX
                case 0x18:
                    st = v[nx];
                    pc += 2;
                    break;
                // ADD I VX
                case 0x1E:
                    // i += v[nx];
                    if (i + v[nx] > 0xFFF) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    i += v[nx];
                    i = (uint16_t) i;
                    pc += 2;
                    break;
                // LOAD F VX
                case 0x29:
                    i = v[nx] * 0x5;
                    pc += 2;
                    break;
                // LOAD B VX
                case 0x33:
                    ram[i] = (uint8_t) ((uint8_t) v[nx] / 100);
                    ram[i + 1] = (uint8_t) ((uint8_t) (v[nx] / 10) % 10);
                    ram[i + 2] = (uint8_t) ((uint8_t) (v[nx] % 100) % 10);
                    pc += 2;
                    break;
                // LOAD I VX
                case 0x55: {
                    for (int k = 0; k <= nx; k++) {
                        ram[i + k] = v[k];
                    }
                    i = i + nx + 1;
                    i = (uint16_t) i;
                    pc += 2;
                    break;
                }
                // LOAD VX I
                case 0x65: {
                    for (int t = 0; t <= nx; t++) {
                        v[t] = ram[i + t];
                    }
                    i = i + nx + 1;
                    i = (uint16_t) i;
                    pc += 2;
                    break;
                }
                // default catchall
                default:
                    std::cerr << "Unknown opcode- " << hex << opcode << endl;
                    break;
            }
            break;
        // default catchall
        default:
            std::cerr << "Unknown opcode- " << hex << opcode << endl;
            break;
    
}
    
    // printf("DT: %.4X\n", dt);        
    if (dt > 0) {
    dt--;
    // cout << hex << dt << endl;
    }
    if (st > 0)
    {
        // st--;
          st = 0;
    }

    
}


Cpu::~Cpu(){}





