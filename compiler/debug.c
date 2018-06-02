//
// Created by Malik Hiraev on 01.06.2018.
//
#include "headers/debug.h"

void print(struct Token *tokens) {
    int d = 0;
    while (tokens[d].type != TEND) {
        struct Token t = tokens[d];
        printf("%d\t%d\t", d, t.line);
        switch (t.type) {
            case ID:
                printf("ID   ");
                break;
            case KWORD:
                printf("KWORD");
                break;
            case LBRC:
                printf("LBRC ");
                break;
            case RBRC:
                printf("RBRC ");
                break;
            case BINOP:
                printf("BINOP");
                break;
            case NUM:
                printf("NUM  ");
                break;
            case SCLN:
                printf("SCLN ");
                break;
            case STR:
                printf("STR  ");
                break;
        }
        printf("\t%s\n", tokens[d].str);
        d++;
    }
}