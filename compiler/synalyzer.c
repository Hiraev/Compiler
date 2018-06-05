//
// Created by Malik Hiraev on 01.06.2018.
//

#include "headers/token.h"
#include "headers/synalyzer.h"
#include "headers/errors.h"

static void str_analyze(struct Token **tokens) {
    unsigned counter = 0;
    while ((*tokens)->type != SCLN) {
        if ((**tokens).type == TEND) printerr(err(SYNTAX_ERROR), "Не найден символ \";\"", (*tokens)->line);
        (*tokens)++;
        counter++;
    }
    if (counter != 4
        || (*tokens)[-3].type != ID
        || strcmp((*tokens)[-2].str, "=")
        || (*tokens)[-1].type != STR) {
        printerr(err(SYNTAX_ERROR), "Ошибка в присваивании строки", (*tokens)->line);
    }
    (*tokens)++;
}

static void print_analyze(struct Token **tokens) {
    unsigned counter = 0;
    while ((**tokens).type != SCLN) {
        if ((**tokens).type == TEND) printerr(err(SYNTAX_ERROR), "Не найден символ \";\"", (*tokens)->line);
        (*tokens)++;
        counter++;
    }
    if (counter != 2
        || !((*tokens)[-1].type == ID
             || (*tokens)[-1].type == STR
             || (*tokens)[-1].type == NUM)
            ) {
        printerr(err(SYNTAX_ERROR), "Ошибка печати", (*tokens)->line);
    }
    (*tokens)++;
}

//TODO
static void assign_analyze(struct Token **tokens) {
    while ((**tokens).type != SCLN) {
        (*tokens)++;
    }
    (*tokens)++;
}

//TODO
static void expr_analyze(struct Token **tokens) {
    while ((**tokens).type != SCLN) {
        (*tokens)++;
    }
    (*tokens)++;
}

void synalyze(struct Token *tokens) {
    //symtab = (struct Sym *) malloc(sizeof(struct Sym) * SIMTAB_SIZE);
    struct Token *current = tokens;
    //Данный цикл проверяет корректность первых токенов
    while (current->type != TEND) {
        switch (current->type) {
            case KWORD:
                if (!strcmp(current->str, "str")) {
                    str_analyze(&current);
                } else if (!strcmp(current->str, "int")) {
                    assign_analyze(&current);
                } else if (!strcmp(current->str, "print")) {
                    print_analyze(&current);
                } else {
                    printerr(err(SYNTAX_ERROR), current->str, current->line);
                }
                continue;
            case ID:
                assign_analyze(&current);
                continue;
            default:
                printerr(err(SYNTAX_ERROR), current->str, current->line);
        }
    }
}