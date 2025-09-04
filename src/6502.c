#include <stdio.h>
#include "6502.h"
#include "instructions.h"

void init_mem(MEM *mem) {
    for (int i = 0; i < MAX_MEM; i++) {
        mem->data[i] = 0;
    }
}

void reset(CPU *cpu, MEM *mem) {
    cpu->PC = 0xFFFC;
    cpu->SP = 0xFF;
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

Word fetch_word(int *cyc, CPU *cpu, MEM *mem) {
    Word data = mem->data[cpu->PC];
    cpu->PC++;
    data |= (mem->data[cpu->PC] << 8);
    cpu->PC++;
    *cyc -= 2;
    return data;
}

Byte read_byte(int *cyc, Byte address, MEM *mem) {
    Byte data = mem->data[address];
    *cyc -= 1;
    return data;
}

Byte read_word(int *cyc, Word address, MEM *mem) {
    Byte data = mem->data[address];
    *cyc -= 1;
    return data;
}

void write_word_stack(int *cyc, CPU *cpu, MEM *mem, Word data) {
    mem->data[0x0100 + cpu->SP] = data & 0xFF;
    cpu->SP--;
    mem->data[0x0100 + cpu->SP] = (data >> 8);
    cpu->SP--;
    *cyc -= 2;
}

void write_byte_stack(int *cyc, CPU *cpu, MEM *mem, Byte data) {
    mem->data[0x0100 + cpu->SP] = data;
    cpu->SP--;
    *cyc -= 1;
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
                         } break;
            case ZP_LDA: {
                             Byte address = fetch_byte(&cycles, cpu, mem);
                             cpu->A = read_byte(&cycles, address, mem);
                             LDA_set_status(cpu);
                         } break;
            case ZPX_LDA: {
                              Byte address = fetch_byte(&cycles, cpu, mem);
                              address += cpu->X;
                              cycles--; // Takes 1 cycle to add X reg
                              cpu->A = read_byte(&cycles, address, mem);
                              LDA_set_status(cpu);
                          } break;
            case AB_LDA: {
                             Word address = fetch_word(&cycles, cpu, mem);
                             cpu->A = read_word(&cycles, address, mem);
                             LDA_set_status(cpu);
                         } break;

            case JSR: {
                          Word address = fetch_word(&cycles, cpu, mem);
                          write_word_stack(&cycles, cpu, mem, cpu->PC - 1);
                          cpu->PC = address;
                          cycles--;
                      } break;
            case IMP_BRK: {
                              fetch_byte(&cycles, cpu, mem); // This is a 2 byte ins just fetching the paddding
                              write_word_stack(&cycles, cpu, mem, cpu->PC);
                              cpu->P |= 0b00010000;
                              write_byte_stack(&cycles, cpu, mem, cpu->P);
                              Word address = 0xFFFE;
                              cpu->PC = (Word) read_word(&cycles, address, mem);
                              address = 0xFFFF;
                              cpu->PC = ((Word) read_word(&cycles, address, mem)) << 8;

                          } break;
            default: {
                         printf("Not handle %d\n", ins);
                     } break;

        }
        if (cycles < 0) {
            printf("Failed no more cycles, c = %d\n", cycles);
        }
    }
}

