//
// Created by Malik Hiraev on 05.06.2018.
//
#include "headers/errors.h"
#include <stdio.h>
#include <stdlib.h>

void printerr(char errmessage[], char string[], unsigned line) {
    printf("%s", errmessage);
    printf("\nВ строке %d: %s", line, string);
    exit(1);
}