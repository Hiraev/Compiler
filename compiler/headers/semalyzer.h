//
// Created by Malik Hiraev on 08.06.2018.
//

#ifndef COMPILER_SEMALYZER_H
#define COMPILER_SEMALYZER_H

#include "token.h"
#include "line.h"
#include "errors.h"
#include "polish_notation.h"
#include "cheker.h"

//Для структуры Sym
enum sym_type {
    INTEGER,
    STRING,
    NO_TYPE
};

//Для семантического анализа
//Для таблицы символов
struct Sym {
    enum sym_type type;
    char *ID; //a link to the char array in the token
};

struct ForGenerator *semanalyze(struct Line *lines);

#endif //COMPILER_SEMALYZER_H