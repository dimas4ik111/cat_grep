#include "s21_cat.h"

int main(int argc, char** argv) {
    s21_cat(argc, argv);
    return 0;
}

void s21_cat(int argc, char** argv) {
    parser* tab;
    tab = calloc(1, sizeof(parser));
    tab_null(tab);
    size_t n = flags_init(argc, argv, tab);
    if (!tab->error) {
        tab->counter = 1;
        for (size_t i = n + 1; (int)i < argc; i++) {
            put_flags_in_text(argv[i], tab);
            #ifdef __APPLE__
            tab->counter = 1;
            #endif
        }
    }
    free(tab);
}

void put_flags_in_text(char* text, parser* tab) {
    FILE* fd = fopen(text, "r");
    if (tab->b)
        tab->n = 0;
    if (fd) {
        char pr_pr = 0;
        char pr_ch = '\n';
        char ch = 0;
        for (int i = 0; (ch = getc(fd)) != EOF; i++) {
            if (tab->s == 's' && flag_s(ch, pr_ch, pr_pr)) {
                continue;
            }
            if (tab->n)
                flag_n(pr_ch, tab, i);
            if (tab->b)
                flag_b(ch, pr_ch, tab, i);
            if (tab->e)
                flag_e(ch);
            if (tab->v && flag_v(ch)) {
                transfer(&pr_pr, &pr_ch, &ch);
                continue;
            }
            if (tab->t && flag_t(ch)) {
                transfer(&pr_pr, &pr_ch, &ch);
                continue;
            }
            if (ch != -1) {
                printf("%c", ch);
                transfer(&pr_pr, &pr_ch, &ch);
            }
        }
        fclose(fd);
    } else {
        printf("No such file %s\n", text);
    }
}

int flag_t(char ch) {
    int check = 0;
    if (ch == 9) {
        printf("^I");
        check = 1;
    }
    return check;
}

void transfer(char* pr_pr, char* pr_ch, char* ch) {
    *pr_pr = *pr_ch;
    *pr_ch = *ch;
}

int flag_v(char ch) {
    int check = 0;
    if (ch != 10 && ch != 9 && ch >= 0 && ch <= 31) {
        printf("^%c", (ch + 64));
        check = 1;
    } else if (ch == 127) {
        printf("^?");
        check = 1;
    }
    return check;
}

void flag_e(char ch) {
    if (ch == '\n')
        printf("$");
}

int flag_s(char ch, char pr_ch, char pr_pr) {
    int check = 0;
    if (pr_pr == '\n' && pr_ch == '\n' && ch == '\n') {
        check = 1;
    }
    return check;
}

void flag_b(char ch, char pr_ch, parser* tab, int i) {
    if (i == 0 && ch != '\n') {
        printf("%6lld\t", tab->counter);
        tab->counter++;
    } else if (pr_ch == '\n' && ch != '\n') {
        printf("%6lld\t", tab->counter);
        tab->counter++;
    }
}

void flag_n(char pr_ch, parser* tab, int i) {
    if (i == 0) {
        printf("%6lld\t", tab->counter);
        tab->counter++;
    } else if (pr_ch == '\n') {
        printf("%6lld\t", tab->counter);
        tab->counter++;
    }
}

size_t flags_init(int argc, char** argv, parser* tab) {
    size_t n = 0;
    for (size_t i = 1; (int)i < argc; i++) {
        if (argv[i][0] == '-') {
            tab_init_flag(argv[i], tab);
            n = i;
        } else {
            break;
        }
    }
    return n;
}

void tab_init_flag(char* fmt, parser* tab) {
    while (*fmt != '\0') {
        switch (*fmt) {
        case 'b':
            tab->b = *fmt;
            break;
        case 'e':
            tab->e = *fmt;
            tab->v = *fmt;
            break;
        case 'n':
            tab->n = *fmt;
            break;
        case 's':
            tab->s = *fmt;
            break;
        case 't':
            tab->t = *fmt;
            tab->v = *fmt;
            break;
        case 'T':
            tab->t = *fmt;
            break;
        case 'v':
            tab->v = *fmt;
            break;
        case 'E':
            tab->e = *fmt;
            break;
        case '-':
            if (strcmp(fmt, "--number-nonblank") == 0) {
                tab->b = 'b';
            } else if (strcmp(fmt, "--number") == 0) {
                tab->n = 'n';
            } else if (strcmp(fmt, "--squeeze-blank") == 0) {
                tab->s = 's';
            }
            fmt += strlen(fmt) - 2;
            break;
        default:
            printf("Invalid argument: %c\n", *fmt);
            tab->error = 1;
            break;
        }
        fmt++;
    }
}

parser* tab_null(parser* tab) {
    tab->b = 0;
    tab->e = 0;
    tab->n = 0;
    tab->s = 0;
    tab->t = 0;
    tab->u = 0;
    tab->v = 0;
    tab->error = 0;
    return tab;
}
