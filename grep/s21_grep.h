#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#define GREP_FLAGS "eivclnhsfo"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <regex.h>
#define _GNU_SOURSE

typedef struct s21_parser {
    int e;
    int i;
    int v;
    int c;
    int l;
    int n;
    int h;
    int f;
    int s;
    int o;
    char** pattern;
    size_t count;
    size_t c_line;
    size_t n_line;
    int error;
    unsigned long long str_counter;
    int flags_res;
    int e_check;
    int fl_count;
} parser_grep;

void s21_grep(int argc, char** argv);
parser_grep* tab_null(parser_grep* tab);
int init_flags(int argc, char** argv, parser_grep* tab);
void init_e(char* buf, parser_grep* tab);
void init_f(char* file_name, parser_grep* tab);
void work_grep(int argc, char** argv, parser_grep* tab);
int file_checker(int argc, char** argv, parser_grep* tab);
void grep_null_flag(int argc, char** argv, parser_grep* tab, int checker);
void pattern_init_without_f_e(int argc, char** argv, parser_grep* tab);
void print_reg(char* buf, int checker, char* argv, parser_grep* tab);
void grep_with_flags(parser_grep* tab, char** argv, int argc, int count_file);
void grep_flag(parser_grep* tab, FILE* fd, int count_file, char* name_file);
void cicle_while(int* flag_l, FILE* fd, parser_grep* tab, int* count_file, char* name_file);

#endif  // SRC_GREP_S21_GREP_H_
