//
// Created by Malik Hiraev on 22.09.2018.
//

#ifndef COMPILER_INSTRUCTION_H
#define COMPILER_INSTRUCTION_H

#include <stdlib.h>

enum instruction {
    PRINT_INT,
    PRINT_IMM,
    PRINT_STR,
    WRITE_INT
};

struct Instr {
    bool immediate;
    enum instruction type;
    unsigned id; //Перемнная, ее номер
    struct Expr *expr;
};

enum expr_elem_type {
    E_OPERATOR,
    E_NUMBER,
    E_ID,
    E_READ,
    E_END
};

enum op {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD
};


struct Expr {
    enum expr_elem_type type;
    union {
        int32_t number;
        unsigned id;
        enum op oper;
    };
};

struct ForGenerator {
    char **strings; //Все строковые константы
    unsigned num_of_strings; //Кол-во строковых переменных
    unsigned num_of_ints; //Кол-во числовых переменных
    unsigned num_of_instructions;
    struct Instr *instructions;
};


#endif //COMPILER_INSTRUCTION_H
