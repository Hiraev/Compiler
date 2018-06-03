//
// Created by Malik Hiraev on 30.05.2018.
//

#include "headers/lexer.h"
#include "headers/errors.h"

#define TRUE 1
#define FALSE 0
#define MAX_BUFF_SIZE 64

struct Token *tokens;

char kwords[][8] = {"int", "str", "print", "read"};

int is_kword(const char *word) {
    for (int i = 0; i < sizeof(kwords); ++i) {
        for (int j = 0; j < sizeof(word); ++j) {
            if (word[j] != kwords[i][j]) break;
            else if (word[j] == '\0') return 1;
        }
    }
    return 0;
}

void printerr(char message[], char sym, unsigned line) {
    printf("%s\n", message);
    printf("В строке %d: %c", line, sym);
    exit(1);
}

int is_num(const char *word) {
    int start = 0;
    if (word[0] == '-') start = 1;
    for (int i = start; i < sizeof(word); ++i) {
        char c = word[i];
        if (c == '\0') return 1;
        if ((c < '0') || (c > '9')) break;
    }
    return 0;
}

int is_delim(char c) {
    if (strchr("; \t\n()+-*/=%|&\"#", c) != NULL) return 1;
    if (strchr("<>", c) != NULL) return 2;
    return 0;
}

void update_mem(unsigned n_tokens, unsigned *mem_size) {
    if (n_tokens + 1 == *mem_size) {
        *mem_size = *mem_size * 2;
        tokens = (struct Token *) realloc(tokens, sizeof(struct Token) * (*mem_size));
    }
}

void save(char *word, unsigned *length, unsigned *wp, unsigned *mem_size, unsigned line_num) {
    if (*length != 0) {
        word[*length] = '\0';
        if (is_kword(word)) {
            tokens[*wp] = (struct Token) {line_num, KWORD};
        } else if (is_num(word)) {
            tokens[*wp] = (struct Token) {line_num, NUM};
        } else {
            tokens[*wp] = (struct Token) {line_num, ID};
        }
        strcpy(tokens[*wp].str, word);
        *length = 0;
        (*wp)++;
        update_mem(*wp, mem_size);
    }
}

struct Token *lexer(FILE *f, unsigned mem_size) {
    tokens = (struct Token *) malloc(sizeof(struct Token) * mem_size);
    char sym;
    unsigned is_comm = FALSE;
    unsigned is_str = FALSE;
    unsigned line_num = 1;
    unsigned length = 0;
    unsigned wp = 0;
    char word[MAX_BUFF_SIZE];
    while ((sym = (char) fgetc(f)) != EOF) {
        char next_sym = (char) getc(f); //Заглядываем, какой символ следующий
        if (next_sym != EOF) fseek(f, -1, SEEK_CUR); //Передвигаем указатель обратно на одну позицию
        int _is_delim = is_delim(sym);
        if (is_comm) {
            if (next_sym == '\n') is_comm = FALSE;
            else continue;
        } else if (is_str) {
            if (sym == '"') {
                is_str = FALSE;
                word[length] = '\0';
                length = 0;
                tokens[wp] = (struct Token) {line_num, STR};
                strcpy(tokens[wp].str, word);
                wp++;
                update_mem(wp, &mem_size);
            } else {
                word[length] = sym;
                length++;
                if (length == MAX_BUFF_SIZE) {
                    printf(err(TOO_LONG_VAR)"\nВ строке %d: %s...", line_num, word);
                    exit(1);
                }
            }
        } else if (_is_delim == 1) {
            save(word, &length, &wp, &mem_size, line_num);
            switch (sym) {
                case '-':
                    if (!(next_sym <= '9' && next_sym >= '0')) {
                        tokens[wp] = (struct Token) {line_num, BINOP, "-"};
                    } else {
                        word[length] = sym;
                        length++;
                        wp--;
                    }
                    break;
                case '+':
                    tokens[wp] = (struct Token) {line_num, BINOP, "+"};
                    break;
                case '*':
                    tokens[wp] = (struct Token) {line_num, BINOP, "*"};
                    break;
                case '/':
                    tokens[wp] = (struct Token) {line_num, BINOP, "/"};
                    break;
                case '%':
                    tokens[wp] = (struct Token) {line_num, BINOP, "%"};
                    break;
                case '|':
                    tokens[wp] = (struct Token) {line_num, BINOP, "|"};
                    break;
                case '&':
                    tokens[wp] = (struct Token) {line_num, BINOP, "&"};
                    break;
                case '=':
                    tokens[wp] = (struct Token) {line_num, BINOP, "="};
                    break;
                case '(':
                    tokens[wp] = (struct Token) {line_num, LBRC, "("};
                    break;
                case ')':
                    tokens[wp] = (struct Token) {line_num, RBRC, ")"};
                    break;
                case ';':
                    tokens[wp] = (struct Token) {line_num, SCLN, ";"};
                    break;
                case '"':
                    is_str = TRUE;
                    wp--;
                    break;
                case '#':
                    is_comm = TRUE;
                    wp--;
                    break;
                case '\n':
                    line_num++;
                case ' ':
                case '\t':
                    wp--;
                    break;
                default:
                    break;
            }
            wp++;
            update_mem(wp, &mem_size);

        } else if (_is_delim == 2) {
            save(word, &length, &wp, &mem_size, line_num);
            switch (sym) {
                case '<':
                    if (next_sym == '<') {
                        tokens[wp] = (struct Token) {line_num, BINOP, "<<"};
                        fseek(f, 1, SEEK_CUR);
                    } else printerr(err(SYNTAX_ERROR), sym, line_num);
                    break;
                case '>':
                    if (next_sym == '>') {
                        tokens[wp] = (struct Token) {line_num, BINOP, ">>"};
                        fseek(f, 1, SEEK_CUR);
                    } else printerr(err(SYNTAX_ERROR), sym, line_num);
                    break;
                default:
                    break;
            }
            wp++;
            update_mem(wp, &mem_size);
        } else {
            word[length] = sym;
            length++;
            if (length == MAX_BUFF_SIZE) {
                printf(err(TOO_LONG_VAR)"\nВ строке %d: %s...", line_num, word);
                exit(1);
            }
            if (next_sym == EOF) save(word, &length, &wp, &mem_size, line_num);
        }
    }
    tokens[wp] = (struct Token) {line_num, TEND};
    return tokens;
}