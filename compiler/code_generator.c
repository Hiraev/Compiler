//
// Created by Malik Hiraev on 23.09.2018.
//
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "headers/code_generator.h"
#include "headers/errors.h"

#define alignment 16 //Выравнивание на 16 байт
#define str_align 8 //Выравнивание строк
#define num_of_sregs 12
#define num_of_aregs 6
//Данное число получено следующим образом:
//Максимальное значение для таких операций как sd, sw, ls, lw - 2048
//Исходя из этих соображений нужно не допустить увеличения размера больше этого значения
//регуистров s - 12, они занимают по 8 байт, плюс регистр ra. Итого 104 байта
//Плюс зарезервированная область для результата scanf - это еще 4 байта
//(2048 - 104 - 4) / 4 = 485 (Значения int занимают 4 байта, поэтому делим на 4)
#define MAX_NUM_OF_VARS 485
#define READ_STATIC_ADDR 0


char *s_regs[] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10",
                  "s11"}; //12 регистров, нужно сохранить
char *a_regs[] = {"a2", "a3", "a4", "a5", "a6", "a7"};


char *get_reg(unsigned reg_index) {
    char *reg = NULL;
    if (reg_index < num_of_aregs) {
        reg = a_regs[reg_index];
    } else {
        reg = s_regs[reg_index];
    }
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

void load_word(FILE *file, char *reg, int32_t id) {
    id = id * 4 + 4;
    fprintf(file, "\tlw  \t\t%s, %d(sp)\n", reg, id);
}

void save_word(FILE *file, char *reg, int32_t id) {
    id = id * 4 + 4;
    fprintf(file, "\tsw  \t\t%s, %d(sp)\n", reg, id);
}

void call_print(FILE *file) {
    fprintf(file, "\tlui \t\ta0, %%hi(.LCO)\n");
    fprintf(file, "\taddi\t\ta0, a0, %%lo(.LCO)\n");
    fprintf(file, "\tcall\t\tprintf\n");
}

void print_int(FILE *file, int32_t id) {
    load_word(file, "a1", id);
    call_print(file);
}

void print_imm(FILE *file, int32_t num) {
    load_immediate(file, "a1", num);
    call_print(file);
}

void print_str(FILE *file, unsigned addr) {
    fprintf(file, "\tlui \t\ta0, %%hi(.LC%d)\n", addr);
    fprintf(file, "\taddi\t\ta0, a0, %%lo(.LC%d)\n", addr);
    fprintf(file, "\tcall\t\tputs\n");
}

void write_strings(FILE *file, char **strings, unsigned num_of_strings) {
    for (int i = 0; i < num_of_strings; ++i) {
        //Здесь плюс 1 - это учет символа конца строки (как в коде от risc-v-gcc)
        size_t length = strlen(strings[i]) + 1;
        size_t align = str_align - (length % str_align);
        if (align == -1) align = 7;
        fprintf(file, ".LC%d:\n", i);
        fprintf(file, "\t.string\t\t\"%s\"\n", strings[i]);
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

void mulw(FILE *file, char *dest, char *src1, char *src2) {
    fprintf(file, "\tmulw\t\t%s, %s, %s\n", dest, src1, src2);
}

void addw(FILE *file, char *dest, char *src1, char *src2) {
    fprintf(file, "\taddw\t\t%s, %s, %s\n", dest, src1, src2);
}

void divw(FILE *file, char *dest, char *src1, char *src2) {
    fprintf(file, "\tdivw\t\t%s, %s, %s\n", dest, src1, src2);
}

void subw(FILE *file, char *dest, char *src1, char *src2) {
    fprintf(file, "\tsubw\t\t%s, %s, %s\n", dest, src1, src2);
}

void remw(FILE *file, char *dest, char *src1, char *src2) {
    fprintf(file, "\tremw\t\t%s, %s, %s\n", dest, src1, src2);
}

void op(FILE *file, char *dest, char *src1, char *src2, enum op op_t) {
    switch (op_t) {
        case ADD:
            addw(file, dest, src1, src2);
            break;
        case SUB:
            subw(file, dest, src1, src2);
            break;
        case MUL:
            mulw(file, dest, src1, src2);
            break;
        case DIV:
            divw(file, dest, src1, src2);
            break;
        case MOD:
            remw(file, dest, src1, src2);
            break;
    }
}

//Safe or load registers
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
    if (!is_save) fprintf(file, "\taddi\t\tsp, sp, %d\n", sp_offset);
}

void printf_scanf_patterns(FILE *file, bool is_print_int, bool is_read) {
    if (is_print_int) {
        fprintf(file, ".LCO:\n");
        fprintf(file, "\t.string\t\t\"%%i\\n\"\n");
        fprintf(file, "\t.zero\t\t3\n");
    }
    if (is_read) {
        fprintf(file, ".LRO:\n");
        fprintf(file, "\t.string\t\t\"%%i\"\n");
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

    if (num_of_ints > MAX_NUM_OF_VARS) {
        fprintf(stderr, "Слишком много перемнных, невозможно записать их в стек.");
        exit(1);
    }

    unsigned reg_index = 0;

    //TODO МОЖНО СНЯТЬ ЭТО ОГРАНИЧЕНИЕ
    if (max_depth > num_of_aregs + num_of_sregs)
        exit_with_msg(ERRM(TOO_MANY_ARGS, "Слишком глубокое выражение"), "", 0);


    bool is_print_int = false;
    bool is_read = false;
    //BEGINNING
    fprintf(file, "\t.text\n");
    fprintf(file, "\t.globl\t\tmain\n");
    fprintf(file, "\t.type\t\tmain, @function\n");
    fprintf(file, "main:\n");
    //SAVING registers in STACK
    unsigned need_to_save = 0;
    if (max_depth > num_of_aregs) need_to_save = max_depth - num_of_aregs;
    sl_registers(file, num_of_ints, need_to_save, true);


    //TODO ANALYZE
    for (int i = 0; i < num_of_instrs; ++i) {
        struct Instr *current = &instrs[i];
        if (current->type == PRINT_INT) {
            print_int(file, current->id);
            is_print_int = true;
        } else if (current->type == PRINT_IMM) {
            print_imm(file, current->expr->number);
        } else if (current->type == PRINT_STR) {
            print_str(file, current->id);
        } else if (current->type == WRITE_INT) {
            struct Expr *expr = current->expr;
            while (expr->type != E_END) {
                if (expr->type == E_NUMBER) {
                    char *reg = get_reg(reg_index);
                    reg_index++;
                    load_immediate(file, reg, expr->number);
                } else if (expr->type == E_ID) {
                    char *reg = get_reg(reg_index);
                    reg_index++;
                    load_word(file, reg, expr->id);
                } else if (expr->type == E_READ) {
                    is_read = true;
                    char *reg = get_reg(reg_index);
                    reg_index++;
                    read(file, READ_STATIC_ADDR);
                    load_word(file, reg, READ_STATIC_ADDR - 1);
                } else if (expr->type == E_OPERATOR) {
                    char *src1 = get_reg(reg_index - 2);
                    reg_index--;
                    char *src2 = get_reg(reg_index);
                    char *dest = src1;
                    op(file, dest, src1, src2, expr->oper);
                }
                expr++;
            }

            char *addr = get_reg(reg_index - 1);
            save_word(file, addr, current->id);
            reg_index--;
        }
    }

    sl_registers(file, num_of_ints, need_to_save, false);
    fprintf(file, "\tjr  \t\tra\n");

    /**
     *              SECTION .rodata
     * */
    fprintf(file, "\t.section\t.rodata\n");
    fprintf(file, "\t.align\t\t3\n");
    //Печатаем шаблоны для чтения и записи числа
    printf_scanf_patterns(file, is_print_int, is_read);
    //Печатаем все строки
    write_strings(file, strings, num_of_strings);
    fclose(file);
}