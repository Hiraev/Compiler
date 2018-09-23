//
// Created by Malik Hiraev on 08.06.2018.
//

#include "headers/semalyzer.h"
#include "headers/instruction.h"

void update_mem(unsigned *size, struct Sym **symtab) {
    *size = *size * 2;
    *symtab = (struct Sym *) realloc(*symtab, sizeof(struct Sym) * *size);
}

void update_ID_map(struct ID_map **idmap, unsigned *size) {
    *size = *size * 2;
    *idmap = (struct ID_map *) realloc(*idmap, sizeof(struct ID_map) * *size);
}

void update_Instr(struct Instr **instrs, unsigned *size) {
    *size = *size * 2;
    *instrs = (struct Instr *) realloc(*instrs, sizeof(struct Instr) * *size);
}

void update_strings(char ***strings, unsigned *size) {
    *size = *size * 2;
    *strings = (char **) realloc(*strings, sizeof(char *) * *size);
}


enum sym_type in_symtab(struct Sym *symtab, char *word, unsigned current_symtab_size) {
    for (unsigned i = 0; i < current_symtab_size; i++) {
        if (!strcmp(word, symtab[i].ID)) return symtab[i].type;
    }
    return NO_TYPE;
}

void add_to_symtab(struct Sym *symtab, enum sym_type type, char *word, unsigned *sym_index) {
    symtab[*sym_index] = (struct Sym) {type, word};
    *sym_index = *sym_index + 1;
}

//Проверка выражения
void check_expr(struct Sym *symtab, struct Token *token, unsigned current_symtab_size, unsigned num_of_line) {
    while (token->type != SCLN) {
        enum token_type type = token->type;
        if (type == ID && in_symtab(symtab, token->str, current_symtab_size) != INTEGER) {
            exit_with_msg(ERRM(BAD_TYPE, "Строка в выражении с целыми числами"), token->str, num_of_line);
        }
        token++;
    }
}


void semanalyze(struct Line *lines) {

    //Для генератора кода
    unsigned num_of_strings = 0; //Кол-во строковых переменных
    unsigned num_of_ints = 0; //Кол-во числовых переменных

    unsigned idmap_size = 10;
    struct ID_map *idmap = (struct ID_map *) malloc(sizeof(struct ID_map) * idmap_size); //инициализируем карту ID

    unsigned inst_size = 10;
    unsigned num_of_instructions = 0;
    struct Instr *instrs = (struct Instr *) malloc(sizeof(struct Instr) * inst_size);
    struct Instr *first_instr = instrs;

    unsigned num_for_string_array = 10;
    unsigned num_of_undefined_strings = 0;
    char **strings = (char **) malloc(sizeof(char *) * num_for_string_array); //Выходные строки

    unsigned symtab_size = 10; ///Размер таблицы(на самом деле просто массив) при инициализации
    unsigned sym_index = 0; //Индекс текущего символа в таблице
    struct Sym *symtab = (struct Sym *) malloc(sizeof(struct Sym) * symtab_size); //инициализируем саму таблицу

    unsigned num_of_line = 1;
    unsigned current_symtab_size = 0; //Сколько идентификаторо сейчас в таблице символов
    struct Line *current = lines; //доставем первую строку
    while (current->type != LEND) { //делаем пока есть строки
        struct Token *token = current->tokens; //Первый токен (int, str, print, ID)

        //ОБНОВЛЯЕМ РАЗМЕРЫ МАССИВОВ
        if (sym_index == symtab_size - 1) { //Если Индекс достиг зачение symtab_syze - 1, пора увеличивать таблицу
            update_mem(&symtab_size, &symtab);
        }
        if (num_of_ints + num_of_strings == idmap_size - 1) {
            update_ID_map(&idmap, &idmap_size);
        }
        if (num_of_instructions == inst_size - 1) {
            update_Instr(&instrs, &inst_size);
        }
        if (num_of_strings == num_for_string_array - 1) {
            update_strings(&strings, &num_for_string_array);
        }


        if (current->type == INT_DEF || current->type == STR_DEF) {
            //В token лежит название переменной
            char *id = (++token)->str;
            //Если переменная уже есть в таблице символов, значит нужно выкинуть ошибку
            if (in_symtab(symtab, id, current_symtab_size) != NO_TYPE) exit_with_msg(ERR(REP_DEF), id, num_of_line);
            //Если это объявление переменной числового типа
            if (current->type == INT_DEF) {
                add_to_symtab(symtab, INTEGER, id, &sym_index);
                token += 2; //Прыгаем к первому токену после знака равно
                check_expr(symtab, token, current_symtab_size, num_of_line);
                idmap[num_of_ints + num_of_strings - num_of_undefined_strings] = (struct ID_map) {id, num_of_ints};

                //TODO to_polish_notation
                //*instrs = (struct Instr) {false, WRITE_INT, num_of_ints, to_polish_notation(token, idmap)};
                num_of_ints++;
            } else if (current->type == STR_DEF) {
                add_to_symtab(symtab, STRING, id, &sym_index);
                idmap[num_of_ints + num_of_strings - num_of_undefined_strings] = (struct ID_map) {id, num_of_strings};
                strings[num_of_strings] = id;
                num_of_strings++;
            }
            current_symtab_size++;
        } else if (current->type == ASSIGNMENT) {
            char *id = token->str;
            enum sym_type s_type = in_symtab(symtab, id, current_symtab_size);
            if (s_type == STRING) exit_with_msg(ERR(CANT_CHANGE_STR), id, num_of_line);
            if (s_type == NO_TYPE) exit_with_msg(ERR(HAVE_TO_DEFINE), id, num_of_line);
            check_expr(symtab, token, current_symtab_size, num_of_line);
            //TODO to_polish_notation
            unsigned index = get_index(idmap, id);
            token += 2; //Прыгаем к первому токену после знака равно
            *instrs = (struct Instr) {false, WRITE_INT, index, to_polish_notation(token, idmap)};
        } else if (current->type == PRINT) {
            enum token_type t_type = (++token)->type;
            if (t_type == STR) {
                strings[num_of_strings] = (token)->str;
                *instrs = (struct Instr) {true, PRINT_STR, num_of_strings, NULL};
                num_of_strings++;
                num_of_undefined_strings++;
            } else if (t_type == ID) {
                unsigned int_index = get_index(idmap, token->str);
                *instrs = (struct Instr) {false, PRINT_INT, int_index, NULL};
            } else if (t_type == NUM) {
                struct Expr *expr = (struct Expr *) malloc(sizeof(struct Expr));
                *expr = (struct Expr) {E_NUMBER, (int32_t) token->num};
                *instrs = (struct Instr) {true, PRINT_INT_IMM, 0, expr};
            }
        }
        //Переходим к следующей строке
        instrs++;
        current++;
        num_of_line++;
    }


    for (int i = 0; i < num_of_line; ++i) {
        switch (first_instr[i].type) {
            case PRINT_INT:
                printf("PRINT_INT: %d\n", first_instr[i].id);
                break;
            case PRINT_INT_IMM:
                printf("PRINT_INT_IMM: %d\n", first_instr[i].expr->number);
                break;
            case PRINT_STR:
                printf("PRINT_STR: %s\n", strings[first_instr[i].id]);
                break;
            case WRITE_INT:
                printf("WRITE_INT: %d ", first_instr[i].id);
                struct Expr *expr = first_instr[i].expr;
                while (expr->type != E_END) {
                    enum expr_elem_type expr_elem_type1 = expr->type;
                    switch(expr_elem_type1) {
                        case E_OPERATOR:
                            printf(" op ");
                            break;
                        case E_NUMBER:
                            printf(" num ");
                            break;
                        case E_ID:
                            printf(" id ");
                            break;
                        case E_READ:
                            printf(" read ");
                            break;
                    }
                    expr++;
                }
                break;
        }
    }

    struct ForGenerator *forGenerator = (struct ForGenerator *) malloc(sizeof(struct ForGenerator));
    forGenerator = (struct ForGenerator *) (strings, num_of_strings, num_of_ints, instrs);

    free(symtab);
}