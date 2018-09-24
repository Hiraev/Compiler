//
// Created by Malik Hiraev on 23.09.2018.
//
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "headers/code_generator.h"

#define alignment 16 //Выравнивание на 16 байт
#define str_align 8 //Выравнивание строк
#define num_of_sregs 12
#define num_of_aregs 6


char *s_regs[] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10",
                  "s11"}; //12 регистров, нужно сохранить
char *a_regs[] = {"a2", "a3", "a4", "a5", "a6", "a7"};


char *get_reg(unsigned *reg_num) {
    char *reg = NULL;
    if (*reg_num < num_of_aregs) {
        reg = a_regs[*reg_num];
    }
    *reg_num += 1;
    return reg;
}

void load_immediate(FILE *file, char *reg, int32_t number) {
    int32_t powed = ((uint32_t) pow(2, 31)) - 2048;
    if (number >= powed) {
        int32_t xori = number ^(int32_t) pow(2, 31);
        fprintf(file, "\tli  \t\t%s, %d\n", reg, (int32_t) -pow(2, 31));
        fprintf(file, "\txori\t\t%s, %s, %d\n", reg, reg, xori);
    } else if (abs(number) > 2048) {
        int32_t rem = number % 2048;
        int32_t res = number - rem;
        fprintf(file, "\tli  \t\t%s, %d\n", reg, res);
        fprintf(file, "\taddi\t\t%s, %s, %d\n", reg, reg, rem);
    } else {
        fprintf(file, "\tli  \t\t%s, %d\n", reg, number);
    }
}

void print_int(FILE *file, int32_t num) {
    load_immediate(file, "a1", num);
    fprintf(file, "\tlui \t\ta0, %%hi(.LCO)\n");
    fprintf(file, "\taddi\t\ta0, a0, %%lo(.LCO)\n");
    fprintf(file, "\tcall\t\tprintf\n");
}

void print_str(FILE *file, unsigned addr) {
    fprintf(file, "\tlui \t\ta0, %%hi(.LC%d)\n", addr);
    fprintf(file, "\taddi\t\ta0, a0, %%lo(.LC%d)\n", addr);
    fprintf(file, "\tcall\t\tprintf\n");
}

void write_strings(FILE *file, char **strings, unsigned num_of_strings) {
    for (int i = 0; i < num_of_strings; ++i) {
        //Здесь плюс 1 - это учет символа конца строки (как в коде от risc-v-gcc)
        size_t length = strlen(strings[i]) + 1;
        size_t align = str_align - (length % str_align);
        align -= 2; //Так как к каждой строке добавляем перенос строки \n
        if (align == -1) align = 7;
        fprintf(file, ".LC%d:\n", i);
        fprintf(file, "\t.string\t\t\"%s\\n\"\n", strings[i]);
        if (align != 8 && align != 0) fprintf(file, "\t.zero\t\t%i\n", (unsigned) align);
    }
}

void read(FILE *file, unsigned addr) {
    //addr - место в стеке, куда сохраним результат (ОТНОСИТЕЛЬНО SP)
    //addi a1, sp, NUMBER - добавляет в a1 адрес, куда нужно сохранить результат scanf
    fprintf(file, "\taddi\t\ta1, sp, %d\n", addr);
    fprintf(file, "\tlui \t\ta0, %%hi(.LRO)\n");
    fprintf(file, "\taddi\t\ta0, a0, %%lo(.LRO)\n");
    fprintf(file, "\tcall\t\tscanf\n");
}

void sl_registers(FILE *file, unsigned num_of_variables, unsigned how_many_sregs, bool is_save) {
    //Регистры будем сохранять с помощью sd, поэтому используем для них 8 bytes
    //А переменные с помощью sw, поэтому для них нужно 4 bytes
    //К num_of_variable прибавлям один для функции scanf, чтобы было куда записыватб результат чтения
    unsigned sp_offset = (how_many_sregs + 1) * 8 + ((num_of_variables + 1) * 4);
    unsigned rem = sp_offset % alignment;
    if (rem != 0) {
        sp_offset += alignment - rem;
    }
    char *command = "ld";
    if (is_save) {
        fprintf(file, "\taddi\t\tsp, sp, %d\n", -sp_offset);
        command = "sd";
    }
    unsigned offset = sp_offset - 8;
    fprintf(file, "\t%s  \t\tra, %d(sp)\n", command, offset);
    offset -= 8;
    for (int i = 0; i < how_many_sregs; ++i) {
        fprintf(file, "\t%s  \t\t%s, %d(sp)\n", command, s_regs[i], offset);
        offset -= 8;
    }
}

void printf_scanf_patterng(FILE *file, bool is_print_int, bool is_read) {
    if (is_print_int) {
        fprintf(file, ".LCO:\n");
        fprintf(file, "\t.string\t\t\"%%i\\n\"\n");
        fprintf(file, "\t.zero\t\t3\n");
    }
    if (is_read) {
        fprintf(file, ".LRO:\n");
        fprintf(file, "\t.string\t\t\"%%i\"");
        fprintf(file, "\t.zero\t\t5\n");
    }
}


void generate(FILE *file, struct ForGenerator *forGenerator) {
    char **strings = forGenerator->strings;
    unsigned max_depth = forGenerator->max_depth;
    unsigned num_of_strings = forGenerator->num_of_strings;
    unsigned num_of_ints = forGenerator->num_of_ints;
    unsigned num_of_instrs = forGenerator->num_of_instructions;
    struct Instr *instrs = forGenerator->instructions;
    bool is_print_int = false;
    bool is_read = false;
    //BEGINNING
    fprintf(file, "\t.text\n");
    fprintf(file, "\t.globl\t\tmain\n");
    fprintf(file, "\t.type\t\tmain, @function\n");
    fprintf(file, "main:\n");
    //SAVE registers in STACK
    sl_registers(file, num_of_ints, 4, true);


    //TODO ANALYZE
    for (int i = 0; i < num_of_instrs; ++i) {
        struct Instr *current = &instrs[i];
        if (current->type == PRINT_INT) {

            is_print_int = true;
        } else if (current->type == PRINT_STR) {
            print_str(file, current->id);
        } else if (current->type == WRITE_INT) {

        }
    }

    sl_registers(file, num_of_ints, 4, false);
    fprintf(file, "\tjr  \t\tra\n");

    /**
     *              SECTION .rodata
     * */
    fprintf(file, "\t.section\t.rodata\n");
    fprintf(file, "\t.align\t\t3\n");
    //Печатаем шаблоны для чтения и записи числа
    printf_scanf_patterng(file, is_print_int, is_read);
    //Печатаем все строки
    write_strings(file, strings, num_of_strings);
    fclose(file);
}