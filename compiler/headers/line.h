//
// Created by Malik Hiraev on 08.06.2018.
//

#ifndef COMPILER_LINE_H
#define COMPILER_LINE_H

#include "token.h"

enum line_type {
    INT_DEF,        // объявление целого числа (int x = ...;)
    STR_DEF,        // объявление строки (str string = ...;)
    ASSIGNMENT,     // присваивание (x = ... ;)
    PRINT,          // печать
    LEND            // конец
};

struct Line {
    enum line_type type;
    struct Token *expr;
};
#endif //COMPILER_LINE_H
