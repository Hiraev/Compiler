//
// Created by Malik Hiraev on 30.05.2018.
//

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H
#define MAX_STR_LENGTH 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum token_type {
    ID,         // идентификатор
    KWORD,      // ключевое слово
    LBRC,       // левая скобка "("
    RBRC,       // правая скобка ")"
    BINOP,      // бинарная операция
    NUM,        // число
    STR,        // строка
    SCLN,       // ";"
    TEND        // метка окончания потока токенов
};

struct Token {
    unsigned line;
    enum token_type type;
    union {
        char str[MAX_STR_LENGTH];
        int32_t num;
    };
};

#endif //COMPILER_TOKEN_H
