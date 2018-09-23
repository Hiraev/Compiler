//
// Created by Malik Hiraev on 23.09.2018.
//

#include "headers/polish_notation.h"


unsigned prior(struct Token *token) {
    //Приоритет операции
    //'(' - 1,
    // '+' и '-' 2,
    // '*' '/' '%' - 3
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

void update_expr(unsigned *size, struct Expr **expr) {
    *size = *size * 2;
    *expr = (struct Expr *) realloc(*expr, sizeof(struct Expr) * (*size));
}

struct Expr *to_polish_notation(struct Token *token, struct ID_map *idmap) {
    unsigned size = 30;
    unsigned num_of_tokens = 0;

    //Стек операций
    struct st *opers = NULL;
    struct Expr *expr = (struct Expr *) malloc(sizeof(struct Expr) * size);
    struct Expr *first_expr = expr;
    while (token->type != SCLN) {
        if (num_of_tokens == size - 1) update_expr(&size, &expr);

        enum token_type t_type = token->type;

        if (t_type == RBRC) {
            while (opers->token->type != LBRC) {
                enum op operation = NULL;
                char *str_oper = del(&opers)->str;
                if (!strcmp(str_oper, "+")) operation = ADD;
                else if (!strcmp(str_oper, "-")) operation = SUB;
                else if (!strcmp(str_oper, "*")) operation = MUL;
                else if (!strcmp(str_oper, "/")) operation = DIV;
                else if (!strcmp(str_oper, "%")) operation = MOD;
                *expr = (struct Expr) {E_OPERATOR, operation};
                num_of_tokens++;
                expr++;
            }
            del(&opers);
        } else if (t_type == ID) {
            unsigned id = get_index(idmap, token->str);
            *expr = (struct Expr) {E_ID, id};
            num_of_tokens++;
            expr++;
        } else if (t_type == NUM) {
            *expr = (struct Expr) {E_NUMBER, token->num};
            num_of_tokens++;
            expr++;
        } else if (t_type == LBRC) {
            opers = push(opers, token);
        } else if (t_type == BINOP) {
            if (opers == NULL) opers = push(opers, token);
            else if (prior(opers->token) < prior(token)) {
                opers = push(opers, token);
            } else {
                while (opers != NULL && prior(opers->token) >= prior(token)) {
                    enum op operation = NULL;
                    char *str_oper = del(&opers)->str;
                    if (!strcmp(str_oper, "+")) operation = ADD;
                    else if (!strcmp(str_oper, "-")) operation = SUB;
                    else if (!strcmp(str_oper, "*")) operation = MUL;
                    else if (!strcmp(str_oper, "/")) operation = DIV;
                    else if (!strcmp(str_oper, "%")) operation = MOD;
                    *expr = (struct Expr) {E_OPERATOR, operation};
                    num_of_tokens++;
                    expr++;
                }
                opers = push(opers, token);
            }
        }
        token++;
    }
    *expr = (struct Expr) {E_END, NULL};
    return first_expr;
}