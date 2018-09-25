//
// Created by Malik Hiraev on 23.05.2018.
//

#ifndef COMPILER_ERRORS_H
#define COMPILER_ERRORS_H
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define NO_FILE "Отсуствует файл для компиляции"
#define BAD_SYMBOL "Символ отсутсвует в алфавите языка"
#define BAD_VAR "Некорректное название переменной"
#define BAD_NUM "Значение числа выходит за допустимые рамки от -2147483648 до 2147483647"
#define BAD_FORMAT "Неверный формат файла"
#define SYNTAX_ERROR "Синтаксическая ошибка"
#define REP_DEF "Повторное объявление переменной"
#define BAD_TYPE "Неверный тип в выражении"
#define HAVE_TO_DEFINE "Необходимо объявить переменную перед ее использованием"
#define CANT_OPEN "Не удалось открыть файл"
#define CANT_CHANGE_STR "Невозможно изменить строку"
#define TOO_LONG_VAR "Слишком длинная строка или название переменной"
#define TOO_MANY_ARGS "Слишком много аргументов"
#define BAD_CALL "Неверный вызов функции"
#define WRONG_EXPANSION "Неправильное расширение файла или его отсутсвие"
#define ERRM(message, other) ANSI_COLOR_RED message". "other ANSI_COLOR_RESET
#define ERR(message) ANSI_COLOR_RED message"."ANSI_COLOR_RESET

void exit_with_msg(char *errmessage, char *string, unsigned line);

#endif //COMPILER_ERRORS_H
