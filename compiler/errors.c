//
// Created by Malik Hiraev on 05.06.2018.
//
#include "headers/errors.h"
#include <stdio.h>
#include <stdlib.h>

void exit_with_msg(char *errmessage, char *string, unsigned line) {
    printf("%s\n", errmessage);
    if (line != 0) printf("В строке %d: ", line);
    printf("%s", string);
    exit(1);
}