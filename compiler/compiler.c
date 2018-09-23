//
// Created by Malik Hiraev on 30.05.2018.
//
#include "headers/lexer.h"
#include "headers/errors.h"
#include "debug/debug.h"
#include "headers/synalyzer.h"
#include "headers/semalyzer.h"

#define DEBUG

int main(int num, char **c) {
    //  Слишком мало аргументов
    if (num < 2) exit_with_msg(ERR(NO_FILE), "Необходимо ввести название файла", 0);

    FILE *file = fopen(c[1], "r");

    //  Не нашли файл
    if (file == NULL) exit_with_msg(ERR(CANT_OPEN), c[1], 0);

    struct Token *tokens = lexer(file);
#ifdef DEBUG
    print(tokens);
#endif
    struct Line *lines = synalyze(tokens);

    //ПОЛУЧАЕМ ВАЛИДНЫЙ КОД, ЕСЛИ ФУНКЦИЯ ОТРАБАТЫВАЕТ УСПЕШНО
    //Такие же Line, что и в предыдущем, только выражения в обратной польской нотации
    struct ForGenerator *forGenerator = semanalyze(lines);
#ifdef DEBUG
    print_2(forGenerator);
#endif
    return 0;
}