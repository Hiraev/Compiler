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

/**
 * ID       - идентификатор
 * KWORD    - ключевое слово
 * LBRC     - левая скобка "("
 * RBRC     - правая скобка ")"
 * BINOP    - бинарная операция
 * NUM      - число
 * STR      - строка
 * SCLN     - ";"
 * TEND     - метка окончания потока токенов
 */
enum token_type {
    ID, KWORD, LBRC, RBRC, BINOP, NUM, STR, SCLN, TEND
};

struct Token {
    unsigned line;
    enum token_type type;
    char str[MAX_STR_LENGTH];
};

#endif //COMPILER_TOKEN_H
