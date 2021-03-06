//
// Created by Malik Hiraev on 30.05.2018.
//

#include <ctype.h>
#include "headers/lexer.h"
#include "headers/errors.h"

#define SAVE_WORD save(&tokens, word, length, tp, line_num); \
                    if (length != 0) { \
                        length = 0; \
                        tp++; \
                        update_mem(&tokens, tp, &mem_size); \
                    }

char kwords[][6] = {"int", "str", "print", "read"};

static bool is_kword(const char *word) {
    for (unsigned i = 0; i < sizeof(kwords) / sizeof(*kwords); ++i) {
        if (!strcmp(kwords[i], word)) return true;
    }
    return false;
}

static bool is_in_range(char *word) {
    long n = strtol(word, &word, 10);
    if (n <= INT32_MAX && n >= INT32_MIN) return true;
    return false;
}

static int32_t to_int32(char *word) { return (int32_t) strtol(word, &word, 10); }

static bool is_num(const char *word) {
    int start = 0;
    if (word[0] == '-') start = 1;
    for (int i = start;; ++i) {
        char c = word[i];
        if (c == '\0') return true;
        if (!isdigit(c)) break;
    }
    return false;
}

static bool is_delim(char c) {
    if (strchr("; \t\n()+-*/=%\"#", c) != NULL) return true;
    return false;
}

static bool is_id(char *id) {
    if (isalpha(*id)) {
        while (isalnum(*id) || *id == '_') {
            id++;
            if (*id == '\0') return true;
        }
    }
    return false;
}

static void update_mem(struct Token **tokens, unsigned n_tokens, unsigned *mem_size) {
    if (n_tokens + 1 == *mem_size) {
        *mem_size = *mem_size * 2;
        *tokens = (struct Token *) realloc(*tokens, sizeof(struct Token) * (*mem_size));
    }
}

static void save(struct Token **tokens, char *word, unsigned length, unsigned tp, unsigned line_num) {
    if (length != 0) {
        word[length] = '\0';
        if (is_kword(word)) {
            (*tokens)[tp] = (struct Token) {line_num, KWORD, {""}};
            strcpy((*tokens)[tp].str, word);
        } else if (is_num(word)) {
            if (!is_in_range(word)) exit_with_msg(ERR(BAD_NUM), word, line_num);
            (*tokens)[tp] = (struct Token) {line_num, NUM, {""}};
            (*tokens)[tp].num = to_int32(word);
        } else if (is_id(word)) {
            (*tokens)[tp] = (struct Token) {line_num, ID, {""}};
            strcpy((*tokens)[tp].str, word);
        } else {
            exit_with_msg(ERR("Лексическая ошибка.\n"
                                      BAD_VAR), word, line_num);
        }
    }
}

struct Token *lexer(FILE *f) {
    unsigned mem_size = 32; //Начальный размер массива токенов
    struct Token *tokens = (struct Token *) malloc(sizeof(struct Token) * mem_size);
    char sym;
    bool is_comm = false;
    bool is_str = false;
    unsigned line_num = 1;
    unsigned length = 0;
    unsigned tp = 0;
    char word[MAX_STR_LENGTH];
    while ((sym = (char) fgetc(f)) != EOF) {
        char next_sym = (char) getc(f); //Заглядываем, какой символ следующий
        if (next_sym != EOF) fseek(f, -1, SEEK_CUR); //Передвигаем указатель обратно на одну позицию
        int _is_delim = is_delim(sym);
        if (is_comm) {
            if (next_sym == '\n') is_comm = false;
            else continue;
        } else if (is_str) {
            if (sym == '"') {
                is_str = false;
                word[length] = '\0';
                length = 0;
                tokens[tp] = (struct Token) {line_num, STR, {""}};
                strcpy(tokens[tp].str, word);
                tp++;
                update_mem(&tokens, tp, &mem_size);
            } else {
                word[length] = sym;
                length++;
                if (length == MAX_STR_LENGTH) {
                    exit_with_msg(ERR("Лексическая ошибка.\n"
                                              TOO_LONG_VAR), word, line_num);
                }
            }
        } else if (_is_delim == 1) {
            SAVE_WORD
            switch (sym) {
                case '-':
                    if (!isdigit(next_sym)) {
                        tokens[tp++] = (struct Token) {line_num, BINOP, {"-"}};
                    } else {
                        word[length] = sym;
                        length++;
                    }
                    break;
                case '+':
                    tokens[tp++] = (struct Token) {line_num, BINOP, {"+"}};
                    break;
                case '*':
                    tokens[tp++] = (struct Token) {line_num, BINOP, {"*"}};
                    break;
                case '/':
                    tokens[tp++] = (struct Token) {line_num, BINOP, {"/"}};
                    break;
                case '%':
                    tokens[tp++] = (struct Token) {line_num, BINOP, {"%"}};
                    break;
                case '=':
                    tokens[tp++] = (struct Token) {line_num, BINOP, {"="}};
                    break;
                case '(':
                    tokens[tp++] = (struct Token) {line_num, LBRC, {"("}};
                    break;
                case ')':
                    tokens[tp++] = (struct Token) {line_num, RBRC, {")"}};
                    break;
                case ';':
                    tokens[tp++] = (struct Token) {line_num, SCLN, {";"}};
                    break;
                case '"':
                    is_str = true;
                    break;
                case '#':
                    is_comm = true;
                    break;
                case '\n':
                    line_num++;
                case ' ':
                case '\t':
                    break;
                default:
                    break;
            }
            update_mem(&tokens, tp, &mem_size);

        } else {
            word[length] = sym;
            length++;
            if (length == MAX_STR_LENGTH) {
                exit_with_msg(ERR("Лексическая ошибка.\n"
                                          TOO_LONG_VAR), word, line_num);
            }
            if (next_sym == EOF) { SAVE_WORD }
        }
    }
    tokens[tp] = (struct Token) {line_num - 1, TEND, {""}};
    return tokens;
}