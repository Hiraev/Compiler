//
// Created by Malik Hiraev on 30.05.2018.
//
#include "headers/lexer.h"
#include "headers/errors.h"
#include "debug/debug.h"
#include "headers/synalyzer.h"
#include "headers/semalyzer.h"
#include "headers/code_generator.h"

//#define DEBUG

int main(int num, char **c) {
    //  Слишком мало аргументов
    if (num < 2) exit_with_msg(ERR(NO_FILE), "Необходимо ввести название файла", 0);

    for (int j = 1; j < num; ++j) {
        FILE *file = fopen(c[j], "r");
        //  Не нашли файл
        if (file == NULL) exit_with_msg(ERR(CANT_OPEN), c[j], 0);

        //Проверяем расширение файла
        unsigned i = 0;
        do {
            i++;
        } while (c[j][i] != '\0');
        //Если i <= 3 значит название файла слишком короткое, чтобы сожержать расширение
        if (i <= 3 || c[j][i - 1] != 'h' || c[j][i - 2] != 'm' || c[j][i - 3] != '.') {
            exit_with_msg(ERR(WRONG_EXPANSION), c[j], 0);
        }

        struct Token *tokens = lexer(file);
#ifdef DEBUG
        print(tokens);
#endif
        struct Line *lines = synalyze(tokens);

        struct ForGenerator *forGenerator = semanalyze(lines);
#ifdef DEBUG
        print_2(forGenerator);
#endif
        char *out = c[j];
        out[i - 2] = 's';
        out[i - 1] = '\0';
        FILE *out_file = fopen(out, "w");
        generate(out_file, forGenerator);
    }
    return 0;
}