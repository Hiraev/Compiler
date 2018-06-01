//
// Created by Malik Hiraev on 30.05.2018.
//

#include "headers/lexer.h"
#include "headers/errors.h"

#define TRUE 1
#define FALSE 0
#define MAX_BUFF_SIZE 64
/*
 * TODO (выделение памяти неправильное)
 * */

char kwords[][8] = {"int", "str", "print"};

int is_kword(const char *word) {
    for (int i = 0; i < sizeof(kwords); ++i) {
        for (int j = 0; j < sizeof(word); ++j) {
            if (word[j] != kwords[i][j]) break;
            else if (word[j] == '\0') return 1;
        }
    }
    return 0;
}

//Только положительное целое число
int is_num(const char *word) {
    for (int i = 0; i < sizeof(word); ++i) {
        char c = word[i];
        if (c == '\0') return 1;
        if ((c < '0') || (c > '9')) break;
    }
    return 0;
}

int is_delim(char c) {
    if (strchr(" \t\n()+-*/%\"", c) != NULL) return 1;
    if (strchr("<>=!|&", c) != NULL) return 2;
    return 0;
}

void update_mem(unsigned n_tokens, unsigned *mem_size, struct Token *tokens) {
    if (n_tokens == *mem_size) {
        *mem_size = *mem_size * 2;
        tokens = (struct Token *) realloc(tokens, *mem_size);
    }
}

void save(char *word, unsigned *length, unsigned *wp, struct Token *tokens, unsigned *mem_size) {
    if (*length != 0) {
        word[*length] = '\0';
        if (is_kword(word)) {
            tokens[*wp] = (struct Token) {KWORD};
        } else if (is_num(word)) {
            tokens[*wp] = (struct Token) {NUM};
        } else {
            tokens[*wp] = (struct Token) {ID};
        }
        strcpy(tokens[*wp].str, word);
        *length = 0;
        (*wp)++;
        update_mem(*wp, mem_size, tokens);
    }
}

struct Token *lexer(FILE *f, unsigned mem_size) {
    struct Token *tokens = (struct Token *) malloc(sizeof(struct Token *) * mem_size);
    char sym;
    unsigned is_str = FALSE;
    unsigned line_num = 1;
    unsigned length = 0;
    unsigned wp = 0;
    char word[MAX_BUFF_SIZE];
    while ((sym = (char) fgetc(f)) != EOF) {
        char next_sym = (char) getc(f); //Заглядываем, какой символ следующий
        if (next_sym != EOF) fseek(f, -1, SEEK_CUR); //Передвигаем указатель обратно на одну позицию
        int _is_delim = is_delim(sym);
        if (is_str) {
            if (sym == '"') {
                is_str = FALSE;
                word[length] = '\0';
                length = 0;
                tokens[wp] = (struct Token) {STR};
                strcpy(tokens[wp].str, word);
                wp++;
                update_mem(wp, &mem_size, tokens);
            } else {
                word[length] = sym;
                length++;
                if (length == MAX_BUFF_SIZE) {
                    printf(err(TOO_LONG_VAR)"\nВ строке %d: %s...", line_num, word);
                    exit(1);
                }
            }
        } else if (_is_delim == 1) {
            save(word, &length, &wp, tokens, &mem_size);
            switch (sym) {
                case '-':
                    tokens[wp] = (struct Token) {BINOP, "-"};
                    break;
                case '+':
                    tokens[wp] = (struct Token) {BINOP, "+"};
                    break;
                case '*':
                    tokens[wp] = (struct Token) {BINOP, "*"};
                    break;
                case '/':
                    tokens[wp] = (struct Token) {BINOP, "/"};
                    break;
                case '%':
                    tokens[wp] = (struct Token) {BINOP, "%"};
                    break;
                case '(':
                    tokens[wp] = (struct Token) {LBRC, "("};
                    break;
                case ')':
                    tokens[wp] = (struct Token) {RBRC, ")"};
                    break;
                case '"':
                    is_str = TRUE;
                    wp--;
                    break;
                case '\n':
                    line_num++;
                case ' ':
                case '\t':
                    wp--;
                    break;
            }
            wp++;
            update_mem(wp, &mem_size, tokens);

        } else if (_is_delim == 2) {
            save(word, &length, &wp, tokens, &mem_size);
            switch (sym) {
                case '|':
                    if (next_sym == '|') {
                        tokens[wp] = (struct Token) {BINOP, "||"};
                        fseek(f, 1, SEEK_CUR);
                    } else tokens[wp] = (struct Token) {BINOP, "|"};
                    break;
                case '&':
                    if (next_sym == '&') {
                        tokens[wp] = (struct Token) {BINOP, "&&"};
                        fseek(f, 1, SEEK_CUR);
                    } else tokens[wp] = (struct Token) {BINOP, "&"};
                    break;
                case '<':
                    if (next_sym == '<') {
                        tokens[wp] = (struct Token) {BINOP, "<<"};
                        fseek(f, 1, SEEK_CUR);
                    } else if (next_sym == '=') {
                        tokens[wp] = (struct Token) {BINOP, "<="};
                        fseek(f, 1, SEEK_CUR);
                    } else tokens[wp] = (struct Token) {BINOP, "<"};
                    break;
                case '>':
                    if (next_sym == '>') {
                        tokens[wp] = (struct Token) {BINOP, ">>"};
                        fseek(f, 1, SEEK_CUR);
                    } else if (next_sym == '=') {
                        tokens[wp] = (struct Token) {BINOP, ">="};
                        fseek(f, 1, SEEK_CUR);
                    } else tokens[wp] = (struct Token) {BINOP, ">"};
                    break;
                case '!':
                    if (next_sym == '=') {
                        tokens[wp] = (struct Token) {BINOP, "!="};
                        fseek(f, 1, SEEK_CUR);
                    } else tokens[wp] = (struct Token) {BINOP, "!"};
                    break;
                case '=':
                    if (next_sym == '=') {
                        tokens[wp] = (struct Token) {BINOP, "=="};
                        fseek(f, 1, SEEK_CUR);
                    } else tokens[wp] = (struct Token) {BINOP, "="};
            }
            wp++;
            update_mem(wp, &mem_size, tokens);
        } else {
            word[length] = sym;
            length++;
            if (length == MAX_BUFF_SIZE) {
                printf(err(TOO_LONG_VAR)"\nВ строке %d: %s...", line_num, word);
                exit(1);
            }
            if (next_sym == EOF) save(word, &length, &wp, tokens, &mem_size);
        }
    }
    tokens[wp] = (struct Token) {TEND};
    return tokens;
}