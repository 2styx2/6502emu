#include <stdlib.h>
#include <stdio.h>
#include "instructions.h"

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

Byte read_byte(int *cyc, Byte address, MEM *mem) {
    Byte data = mem->data[address];
    *cyc -= 1;
    return data;
}

void LDA_set_status(CPU *cpu) {
    if (cpu->A == 0) {
        cpu->P |= 0b00000010;
    }
    if (cpu->A & 0b10000000) {
        cpu->P |= 0b10000000;
    }
}
void execute(int cycles, CPU *cpu, MEM *mem) {
    while (cycles > 0) {
        Byte ins = fetch_byte(&cycles, cpu, mem);
        switch (ins) {
            case IM_LDA: {
                             Byte val = fetch_byte(&cycles, cpu, mem);
                             cpu->A = val;
                             LDA_set_status(cpu);
                         }
            case ZP_LDA: {
                             Byte address = fetch_byte(&cycles, cpu, mem);
                             cpu->A = read_byte(&cycles, address, mem);
                             LDA_set_status(cpu);
                         }
            case ZPX_LDA: {
                              Byte address = fetch_byte(&cycles, cpu, mem);
                              address += cpu->X;
                              cycles--; // Takes 1 cycle to add X reg
                              cpu->A = read_byte(&cycles, address, mem);
                              LDA_set_status(cpu);
                          }

            default: {
                         printf("Not handle %d\n", ins);
                     } break;

        }
        if (cycles < 0) {
            printf("Failed no more cycles, c = %d\n", cycles);
        }
    }
}

int main(int argc, char* argv[]) {
    MEM mem;
    CPU cpu;
    reset(&cpu, &mem);
    int cycles = 2;
    cpu.X = 0x0F;
    mem.data[0xFFFC] = ZPX_LDA;
    mem.data[0xFFFD] = 0x80;
    mem.data[0x008F] = 0x44;
    execute(4, &cpu, &mem);
    printf("A: %X P: %X PC: %X, SP: %X\n",cpu.A, cpu.P, cpu.PC, cpu.SP);
    return 0;
}

