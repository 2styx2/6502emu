#include <stdio.h>
#include "6502.h"
#include "instructions.h"

int main(int argc, char* argv[]) {
    MEM mem;
    CPU cpu;
    reset(&cpu, &mem);
    int cycles = 2;
    cpu.X = 0x0F;
    mem.data[0xFFFC] = JSR;
    mem.data[0xFFFD] = 0x44;
    mem.data[0xFFFE] = 0x80;
    mem.data[0x8044] = ZP_LDA;
    mem.data[0x8045] = 0x22;
    mem.data[0x0022] = 0x69;
    execute(9, &cpu, &mem);
    printf("A: %X P: %X PC: %X, SP: %X\n",cpu.A, cpu.P, cpu.PC, cpu.SP);
    return 0;
}

