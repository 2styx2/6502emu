#ifndef CPU_6502_H
#define CPU_6502_H

typedef unsigned char Byte;
typedef unsigned short Word;

#define MAX_MEM 1024 * 64

typedef struct {
    Byte A, X, Y; // Regs
    Byte SP;
    Word PC;
    Byte P;
} CPU;

typedef struct {
    Byte data[MAX_MEM];
} MEM;

void init_mem(MEM *mem);

void reset(CPU *cpu, MEM *mem);

Byte fetch_byte(int *cyc, CPU *cpu, MEM *mem);

Word fetch_word(int *cyc, CPU *cpu, MEM *mem);

Byte read_byte(int *cyc, Byte address, MEM *mem);

void write_word_stack(int *cyc,CPU *cpu, MEM *mem, Word data);

void LDA_set_status(CPU *cpu);

void execute(int cycles, CPU *cpu, MEM *mem);

#endif
