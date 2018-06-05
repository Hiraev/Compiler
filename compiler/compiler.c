//
// Created by Malik Hiraev on 30.05.2018.
//
#include "headers/lexer.h"
#include "headers/errors.h"
#include "debug/debug.h"
#include "headers/synalyzer.h"
int main(int num, char **c) {
    //  Слишком мало аргументов
    if (num < 2) {
        printf(err(NO_FILE));
        return 1;
    }

    FILE *f = fopen(c[1], "r");

    //  Не нашли файл
    if (f == NULL) {
        printf(err(CANT_OPEN)"\n");
        printf(c[1]);
        return 1;
    }

    struct Token *tokens = lexer(f, 12);
    print(tokens);
    return 0;
}