//
// Created by Malik Hiraev on 01.06.2018.
//
#include "debug.h"

#define PRINT(id, ids) case id: printf("%-5s", ids); break;

void print(struct Token *tokens) {
    int d = 0;
    while (tokens[d].type != TEND) {
        struct Token t = tokens[d];
        printf("%d\t%d\t", d, t.line);
        switch (t.type) {
            PRINT(ID, "ID")
            PRINT(KWORD, "KWORD")
            PRINT(LBRC, "LBRC")
            PRINT(RBRC, "RBRC")
            PRINT(BINOP, "BINOP")
            PRINT(NUM, "NUM")
            PRINT(SCLN, "SCLN")
            PRINT(STR, "STR")
        }
        if (t.type != NUM) printf("\t%s\n", tokens[d].str);
        else printf("\t%d\n", tokens[d].num);
        d++;
    }
}