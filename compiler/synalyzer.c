//
// Created by Malik Hiraev on 01.06.2018.
//

#include "headers/token.h"
#include "headers/synalyzer.h"
#include "headers/errors.h"

#define DEF 1 //Если это объявление
#define NO_DEF 0 //Если просто присваивание

static void update_mem(struct Line **lines, unsigned n_lines, unsigned *mem_size) {
    if (n_lines + 1 == *mem_size) {
        *mem_size = *mem_size * 2;
        *lines = (struct Line *) realloc(*lines, sizeof(struct Line) * *mem_size);
    }
}

static bool is_id_num_read(struct Token *t) {
    return t->type == ID
           || t->type == NUM
           || !strcmp(t->str, "read");
}

static bool is_binop(struct Token *t) {
    return t->type == BINOP && strcmp(t->str, "=");
}

static void str_analyze(struct Token **tokens) {
    unsigned counter = 0;
    while ((*tokens)->type != SCLN) {
        if ((**tokens).type == TEND) exit_with_msg(ERR(SYNTAX_ERROR), "Не найден символ \";\"", (*tokens)->line);
        (*tokens)++;
        counter++;
    }
    if (counter != 4
        || (*tokens)[-3].type != ID
        || strcmp((*tokens)[-2].str, "=")
        || (*tokens)[-1].type != STR) {
        exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка в присваивании строки", (*tokens)->line);
    }
    (*tokens)++;
}

static void print_analyze(struct Token **tokens) {
    unsigned counter = 0;
    while ((**tokens).type != SCLN) {
        if ((**tokens).type == TEND) exit_with_msg(ERR(SYNTAX_ERROR), "Не найден символ \";\"", (*tokens)->line);
        (*tokens)++;
        counter++;
    }
    if (counter != 2
        || !((*tokens)[-1].type == ID
             || (*tokens)[-1].type == STR
             || (*tokens)[-1].type == NUM)
            ) {
        exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка печати", (*tokens)->line);
    }
    (*tokens)++;
}

/*
 * КОНЕЧНЫЙ АВТОМАТ С 3-мя СОСТОЯНИЯМИ
 * далее под BINOP подразумевается {BINOP}/{"="}
 * под READ подразумевается KWORD: "read"
 * init - точка входа в автомат
 * exit - точка выхода из автомата
 *
 * Переходы:
 * init     NUM, ID, READ     ->  0
 * init     LBRC              ->  1
 * 0        BINOP             ->  1
 * 1        LBRC              ->  1
 * 1        NUM, ID, READ     ->  2
 * 2        BINOP             ->  1
 * 2        RBRC              ->  3
 * 3        RBRC              ->  3
 * 3        BINOP             ->  1
 * 3        SCLN              ->  exit
 * 2        SCLN              ->  exit
 * 0        SCLN              ->  exit
 */
static void expr_analyze(struct Token **tokens) {
    short state = 0;
    unsigned lbrc = 0;
    unsigned rbrc = 0;
    // init
    if ((**tokens).type == LBRC) {
        state = 1;
        lbrc++;
        (*tokens)++;
    } else if (is_id_num_read(*tokens)) {
        (*tokens)++;
        if ((**tokens).type == SCLN) {
            (*tokens)++;
            return;
        } else if ((**tokens).type == BINOP && strcmp((**tokens).str, "=")) {
            state = 1;
        } else exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
        (*tokens)++;
    } else exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);

    //Переходы между состояниями
    while ((**tokens).type != SCLN) {
        if ((*tokens)->type == TEND) exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
        if (state == 1) {
            if (is_id_num_read(*tokens)) {
                state = 2;
            } else if ((**tokens).type == LBRC) lbrc++;
            else exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
        } else if (state == 2) {
            if ((**tokens).type == RBRC) {
                if (lbrc <= rbrc)
                    exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения. Закравающая скобка без пары", (*tokens)->line);
                state = 3;
                rbrc++;
            } else if (is_binop(*tokens)) {
                state = 1;
            } else exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
        } else if (state == 3) {
            if ((**tokens).type == RBRC) {
                if (lbrc <= rbrc)
                    exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения. Закравающая скобка без пары", (*tokens)->line);
                rbrc++;
            } else if (is_binop(*tokens)) {
                state = 1;
            } else exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
        }
        (*tokens)++;
    }
    if (state == 1) exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
    if (lbrc != rbrc)
        exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения. Несоответствие кол-ва открывающих и закрывающих скобок",
                      (*tokens)->line);
    (*tokens)++;
}

static void assign_analyze(struct Token **tokens, unsigned is_def) {
    (*tokens)++;
    if ((*tokens)->type == TEND) exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
    if (is_def) (*tokens)++;
    if (((**tokens).type != BINOP || strcmp((**tokens).str, "=")) ||
        (*(++(*tokens))).type == SCLN)
        exit_with_msg(ERR(SYNTAX_ERROR), "Ошибка выражения", (*tokens)->line);
    expr_analyze(tokens);
}

struct Line *synalyze(struct Token *tokens) {
    unsigned mem_size = 10;
    unsigned line_num = 0;
    struct Line *lines = (struct Line *) malloc(sizeof(struct Line) * mem_size);
    struct Token *current = tokens;
    //Данный цикл проверяет корректность первых токенов
    while (current->type != TEND) {
        switch (current->type) {
            case KWORD:
                if (!strcmp(current->str, "str")) {
                    lines[line_num] = (struct Line) {STR_DEF, current};
                    str_analyze(&current);
                } else if (!strcmp(current->str, "int")) {
                    lines[line_num] = (struct Line) {INT_DEF, current};
                    assign_analyze(&current, DEF);
                } else if (!strcmp(current->str, "print")) {
                    lines[line_num] = (struct Line) {PRINT, current};
                    print_analyze(&current);
                } else {
                    exit_with_msg(ERR(SYNTAX_ERROR), current->str, current->line);
                }
                break;
            case ID:
                lines[line_num] = (struct Line) {ASSIGNMENT, current};
                assign_analyze(&current, NO_DEF);
                break;
            default:
                exit_with_msg(ERR(SYNTAX_ERROR), current->str, current->line);
        }
        update_mem(&lines, line_num + 1, &mem_size);
        line_num++;
    }
    lines[line_num] = (struct Line) {LEND};
    return lines;
}