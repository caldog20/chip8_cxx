#ifndef chip8_h
#define chip8_h

#include "global.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

class Cpu {
  public:
    Byte v[16];
    Word i, pc;
    Byte sp;
    Word stack[16];
    Byte dt, st;
    Byte ram[4096];
    Word input[16];
    bool draw;
    Word counter;
    int display[64 * 32];

    Cpu();

    bool loadRom(std::string);
    void executeOpcode(bool);

    ~Cpu();

  private:
    void loadFonts();
    Word fetchOpcode(Word);
};

#endif