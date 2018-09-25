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

void print_2(struct ForGenerator *forGenerator) {

    struct Instr* first_instr = forGenerator->instructions;
    unsigned num_of_strings = forGenerator->num_of_strings;
    char **strings = forGenerator->strings;

    printf("\n\nALL STRINGS:\n");
    for (int i = 0; i < num_of_strings; i++) {
        printf("%s\n", strings[i]);
    }

    for (int i = 0; i < forGenerator->num_of_instructions; ++i) {
        printf("%d ", i + 1);
        switch (first_instr[i].type) {
            case PRINT_INT:
                printf("PRINT_INT: ID : %d\n", first_instr[i].id);
                break;
            case PRINT_IMM:
                printf("PRINT_IMM: VAL: %d\n", first_instr[i].expr->number);
                break;
            case PRINT_STR:
                printf("PRINT_STR: ID : %s\n", strings[first_instr[i].id]);
                break;
            case WRITE_INT:
                printf("WRITE_INT: ID : %d | ", first_instr[i].id);
                struct Expr *expr = first_instr[i].expr;
                while (expr->type != E_END) {
                    enum expr_elem_type expr_elem_type1 = expr->type;
                    switch (expr_elem_type1) {
                        case E_OPERATOR:
                            switch (expr->oper) {
                                case ADD:
                                    printf(" ADD ");
                                    break;
                                case SUB:
                                    printf(" SUB ");
                                    break;
                                case MUL:
                                    printf(" MUL ");
                                    break;
                                case DIV:
                                    printf(" DIV ");
                                    break;
                                case MOD:
                                    printf(" MOD ");
                                    break;
                            }
                            break;
                        case E_NUMBER:
                            printf(" %d ", expr->number);
                            break;
                        case E_ID:
                            printf(" (id: %d) ", expr->id);
                            break;
                        case E_READ:
                            printf(" (READ) ");
                            break;
                    }
                    expr++;
                }
                printf("\n");
                break;
        }
    }
}