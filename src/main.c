#include <stdio.h>
#include "6502.h"
#include "instructions.h"

int main(int argc, char* argv[]) {
    MEM mem;
    CPU cpu;
    reset(&cpu, &mem);
    mem.data[0xFFFC] = IMP_BRK;
    mem.data[0xFFFD] = 0x00;
    mem.data[0xFFFE] = 0x00;
    mem.data[0xFFFF] = 0xF0;
    mem.data[0xF000] = 0xE4;
    execute(8, &cpu, &mem);
    printf("A: 0x%X P: 0x%X PC: 0x%X, SP: 0x%X\n",cpu.A, cpu.P, cpu.PC, cpu.SP);
    return 0;
}

