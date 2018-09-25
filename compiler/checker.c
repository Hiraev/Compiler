//
// Created by Malik Hiraev on 23.09.2018.
//
#include "headers/checker.h"

unsigned get_index(struct ID_map *id_map, char *id, unsigned id_map_size) {
    for (int i = 0; i < id_map_size; ++i) {
        if (id_map->id == NULL) break;
        if (!strcmp(id_map->id, id))
            return id_map->index;
        id_map++;
    }
    exit_with_msg(ERR(HAVE_TO_DEFINE), "", 0);
    return 0; //Этого не происзойдет, так как предыдущий вызов выполнит exit(1)
}