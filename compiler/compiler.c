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
        exit_with_msg(ERR(NO_FILE), "Необходимо ввести название файла", 0);
    }

    FILE *f = fopen(c[1], "r");

    //  Не нашли файл
    if (f == NULL) {
        exit_with_msg(ERR(CANT_OPEN), c[1], 0);
    }

    struct Token *tokens = lexer(f, 12);
#ifdef DEBUG
    print(tokens);
#endif
    synalyze(tokens);
    return 0;
}