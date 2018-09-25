//
// Created by Malik Hiraev on 23.09.2018.
//

#ifndef COMPILER_CHEKER_H
#define COMPILER_CHEKER_H

#include "token.h"
#include "instruction.h"
#include "errors.h"

struct ID_map {
    char *id;
    unsigned index; //Позиция в массиве
};


unsigned get_index(struct ID_map *id_map, char *id, unsigned id_map_size);

#endif //COMPILER_CHEKER_H
