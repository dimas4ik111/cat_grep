#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#define CAT_FLAGS "benstuv"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct s21_parser {
    int b;
    int e;
    int n;
    int s;
    int t;
    int u;
    int v;
    long long int counter;
    int error;
} parser;

void s21_cat(int argc, char** argv);
parser* tab_null(parser* tab);
void tab_init_flag(char* fmt, parser* tab);
size_t flags_init(int argc, char** argv, parser* tab);
void put_flags_in_text(char* text, parser* tab);
int flag_s(char ch, char pr_ch, char pr_pr);
void flag_b(char ch, char pr_ch, parser* tab, int i);
void flag_n(char pr_ch, parser* tab, int i);
void flag_e(char ch);
void transfer(char* pr_pr, char* pr_ch, char* ch);
int flag_v(char ch);
int flag_t(char ch);

#endif  // SRC_CAT_S21_CAT_H_
