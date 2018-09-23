//
// Created by Malik Hiraev on 23.09.2018.
//

#ifndef COMPILER_POLISH_NOTATION_H
#define COMPILER_POLISH_NOTATION_H

#include "token.h"
#include "instruction.h"
#include "cheker.h"
//Стек
struct st {
    struct Token *token;
    struct st *next;
};

struct Expr *to_polish_notation(struct Token *token, struct ID_map *idmap);

#endif //COMPILER_POLISH_NOTATION_H
