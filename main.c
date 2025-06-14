#include <stdlib.h>
#include <stdio.h>

typedef unsigned char Byte;
typedef unsigned short Word;
#define MAX_MEM 1024 * 64

typedef struct {
    Byte A, X, Y; // Regs
    Word SP;
    Word PC;
    Byte P;
} CPU;

typedef struct {
    Byte data[MAX_MEM];
} MEM;

void init_mem(MEM *mem) {
    for (int i = 0; i < MAX_MEM; i++) {
        mem->data[i] = 0;
    }
}

void reset(CPU *cpu, MEM *mem) {
    cpu->PC = 0xFFFC;
    cpu->SP = 0x01FF;
    cpu->P = 0;
    cpu->A, cpu->X, cpu->Y = 0;
    init_mem(mem);
}

Byte fetch_byte(int *cyc, CPU *cpu, MEM *mem) {
    Byte data = mem->data[cpu->PC];
    cpu->PC++;
    *cyc -= 1;
    return data;
}

void execute(int cycles, CPU *cpu, MEM *mem) {
    while (cycles > 0) {
        Byte ins = fetch_byte(&cycles, cpu, mem);
    }
}

int main(int argc, char* argv[]) {
    MEM mem;
    CPU cpu;
    reset(&cpu, &mem);
    int cycles = 2;
    execute(2, &cpu, &mem);
    printf("PC: %X, SP: %X\n", cpu.PC, cpu.SP);
    return 0;
}

