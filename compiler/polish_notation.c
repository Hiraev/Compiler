//
// Created by Malik Hiraev on 23.09.2018.
//

#include "headers/polish_notation.h"


unsigned prior(struct Token *token) {
    //Приоритет операции '(' - 1, '+' и '-' 2, '*' '/' '%' - 3
    if (token->type == LBRC) return 1;
    if (!strcmp(token->str, "+") || !strcmp(token->str, "-")) return 2;
    return 3;
}

struct st *push(struct st *head, struct Token *t) {
    struct st *ptr = (struct st *) malloc(sizeof(struct st));
    ptr->token = t;
    ptr->next = head;
    return ptr; // Новая вершина стека
}

//Возвращает удаленный токен
struct Token *del(struct st **head) {
    struct st *ptr;
    struct Token *t;
    if (*head == NULL) return NULL;
    ptr = *head;
    t = ptr->token;
    *head = ptr->next;
    free(ptr);
    return t;
}

void update_expr(struct Expr **expr, unsigned *size) {
    *size = *size * 2;
    *expr = (struct Expr *) realloc(*expr, sizeof(struct Expr) * (*size));
}

void choose_op(struct st **opers, struct Expr *expr, unsigned expr_index) {
    enum op operation = NULL;
    char *str_oper = del(opers)->str;
    if (!strcmp(str_oper, "+")) operation = ADD;
    else if (!strcmp(str_oper, "-")) operation = SUB;
    else if (!strcmp(str_oper, "*")) operation = MUL;
    else if (!strcmp(str_oper, "/")) operation = DIV;
    else if (!strcmp(str_oper, "%")) operation = MOD;
    else exit(1); //ЭТОГО НИКОГДА НЕ ДОЛЖНО СЛУЧИТЬСЯ, НИКОГДА!!!!!!!!!!!!!!
    expr[expr_index] = (struct Expr) {E_OPERATOR, operation};
}

struct Expr *to_polish_notation(struct Token *token, struct ID_map *idmap) {
    unsigned size = 10;

    //Стек операций
    struct st *opers = NULL;
    unsigned expr_index = 0;
    struct Expr *expr = (struct Expr *) malloc(sizeof(struct Expr) * size);

    while (token->type != SCLN) {
        if (expr_index == size - 1) update_expr(&expr, &size);
        enum token_type t_type = token->type;

        if (t_type == RBRC) {
            while (opers->token->type != LBRC) {
                choose_op(&opers, expr, expr_index);
                expr_index++;
            }
            del(&opers);
        } else if (t_type == ID) {
            unsigned id = get_index(idmap, token->str);
            expr[expr_index] = (struct Expr) {E_ID, id};
            expr_index++;
        } else if (t_type == KWORD) { //ТАК УВЕРЕННО ПИШЕМ E_READ, ПОТОМУ что предыдущие стадии гарантируют, что тут не
            //будет ничего другого
            expr[expr_index] = (struct Expr) {E_READ, -1};
            expr_index++;
        } else if (t_type == NUM) {
            expr[expr_index] = (struct Expr) {E_NUMBER, token->num};
            expr_index++;
        } else if (t_type == LBRC) {
            opers = push(opers, token);
        } else if (t_type == BINOP) {
            if (opers == NULL) opers = push(opers, token);
            else if (prior(opers->token) < prior(token)) {
                opers = push(opers, token);
            } else {
                while (opers != NULL && prior(opers->token) >= prior(token)) {
                    choose_op(&opers, expr, expr_index);
                    expr_index++;
                }
                opers = push(opers, token);
            }
        }
        token++;
    }
    while (opers != NULL) {
        choose_op(&opers, expr, expr_index);
        expr_index++;
    }
    expr[expr_index] = (struct Expr) {E_END, NULL};
    return expr;
}