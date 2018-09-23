//
// Created by Malik Hiraev on 23.09.2018.
//
#include "headers/checker.h"

unsigned get_index(struct ID_map *id_map, char *id) {
    while (true) {
        if (!strcmp(id_map->id, id))
            return id_map->index;
        id_map++;
    }
}