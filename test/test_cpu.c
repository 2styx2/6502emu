#include "unity.h"
#include "instructions.h"
#include "6502.h"   // assumes your Makefile adds -Isrc so this resolves

static CPU cpu;
static MEM mem;

void setUp(void) {
    // Start from known state
    init_mem(&mem);
    cpu.A = cpu.X = cpu.Y = 0;
    cpu.SP = 0;
    cpu.PC = 0;
    cpu.P  = 0;
}

void tearDown(void) {}

// Check that init_mem clears memory bytes (spot-check a few positions)
void test_init_mem_zeros_memory(void) {
    mem.data[0]        = 0xFF;
    mem.data[1]        = 0xFF;
    mem.data[0x1234]   = 0xFF;
    mem.data[0xFFFF]   = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0xFF, mem.data[0]);

    init_mem(&mem);

    TEST_ASSERT_EQUAL_UINT8(0, mem.data[0]);
    TEST_ASSERT_EQUAL_UINT8(0, mem.data[1]);
    TEST_ASSERT_EQUAL_UINT8(0, mem.data[0x1234]);
    TEST_ASSERT_EQUAL_UINT8(0, mem.data[0xFFFF]);
}

void test_im_lda(void) {
    reset(&cpu, &mem);
    mem.data[0xFFFC] = 0xa9;
    mem.data[0xFFFD] = 0x69;
    execute(2, &cpu, &mem);
    TEST_ASSERT_EQUAL_UINT8(0x69, cpu.A);
}

void test_zpx_lda(void) {
    reset(&cpu, &mem);
    cpu.X = 0xFF;
    mem.data[0xFFFC] = 0xB5;
    mem.data[0xFFFD] = 0x80;
    mem.data[0x007F] = 0x22;
    execute(4, &cpu, &mem);
    TEST_ASSERT_EQUAL_UINT8(0x22, cpu.A);
}


void test_ab_lda(void) {
    reset(&cpu, &mem);
    mem.data[0xFFFC] = 0xAD;
    mem.data[0xFFFD] = 0x10;
    mem.data[0xFFFE] = 0x30;
    mem.data[0x3010] = 0x34;
    execute(4, &cpu, &mem);
    TEST_ASSERT_EQUAL_UINT8(0x34, cpu.A);
}

void test_jsr(void) {
    reset(&cpu, &mem);
    mem.data[0xFFFC] = 0x20;
    mem.data[0xFFFD] = 0x10;
    mem.data[0xFFFE] = 0x30;
    execute(6, &cpu, &mem);
    TEST_ASSERT_EQUAL_UINT16(0x3010, cpu.PC);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_mem_zeros_memory);
    RUN_TEST(test_im_lda);
    RUN_TEST(test_jsr);
    RUN_TEST(test_ab_lda);
    RUN_TEST(test_zpx_lda);
    return UNITY_END();
}

