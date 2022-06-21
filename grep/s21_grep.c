#include "s21_grep.h"

int main(int argc, char** argv) {
    if (argc > 1) {
        s21_grep(argc, argv);
    } else {
        printf("usage: s21_grep [pattern] [file] [flags]\n");
    }
    return 0;
}

void s21_grep(int argc, char** argv) {
    parser_grep* tab;
    tab = calloc(1, sizeof(parser_grep));
    tab_null(tab);
    tab->pattern = (char**)calloc(1, sizeof(char*));
    int flag_null = init_flags(argc, argv, tab);
    if (flag_null == 0) {  // situation without flags, pattern is first argumment
        tab->flags_res = 1;
        tab->pattern[0] = argv[1];
    } else if (!tab->f && !tab->e) {  // if any flags, but are not 'f' and 'e'
        pattern_init_without_f_e(argc, argv, tab);
    }
    if (tab->error != -1) {  // if no errors, programm start grep
        work_grep(argc, argv, tab);
    }
    if (tab->f || tab->e) {
        for (size_t i = 0; i < tab->count; i++) {
            free(tab->pattern[i]);
        }
    }
    free(tab->pattern);
    free(tab);
}

void work_grep(int argc, char** argv, parser_grep* tab) {
    int counter_files = file_checker(argc, argv, tab);  // counter_files;
    tab->fl_count = counter_files;
    if (tab->flags_res == 1 && counter_files != 0) {  // work situation, without flags
        grep_null_flag(argc, argv, tab, counter_files);
    } else if (counter_files != 0) {  // situation with any flags
        grep_with_flags(tab, argv, argc, counter_files);
    }
    if (counter_files == 0 && !tab->s) {
        printf("NO FILES to s21_grep\n");
    }
}

void grep_with_flags(parser_grep* tab, char** argv, int argc, int count_file) {
    int check_pat = 0;
    if (tab->f || tab->e)
        check_pat++;
    for (int i = 1; i < argc; i++) {
        if (argv[i - 1][0] == '-' && !tab->e_check && (strchr(argv[i - 1], 'f') != NULL
        || strchr(argv[i - 1], 'e') != NULL))
            i++;
        if (i >= argc)
            break;
        if (argv[i][0] != '-') {
            check_pat++;
            if (check_pat > 1) {
                FILE* fd;
                fd = fopen(argv[i], "r");
                if (fd) {
                    tab->fl_count--;
                    grep_flag(tab, fd, count_file, argv[i]);
                    fclose(fd);
                } else if (!tab->s && !tab->l && !tab->h) {
                    fprintf(stderr, "s21_grep: %s: no such file or directory\n", *(argv + i));
                }
            }
        }
    }
}

void grep_flag(parser_grep* tab, FILE* fd, int count_file, char* name_file) {
    int flag_l = 0;
    cicle_while(&flag_l, fd, tab, &count_file, name_file);
    #ifdef __APPLE__
    if (tab->c) {
        if (flag_l && tab->l)
            tab->c_line = 1;
        if (count_file > 1 && !tab->h)
            printf("%s:", name_file);
        printf("%ld\n", tab->c_line);
    }
    if (tab->l && flag_l)
        printf("%s\n", name_file);
    // #elif __linux__
    // if (tab->c && !tab->l) {
    //     if (count_file > 1 && !tab->h)
    //         printf("%s:", name_file);
    //     printf("%ld\n", tab->c_line);
    // }
    // if (tab->l && flag_l)
    //     printf("%s\n", name_file);
    // if (tab->l && tab->c && !tab->c_line) {
    //     if (!tab->h) {
    //         printf("%s:", name_file);
    //         printf("%ld\n", tab->c_line);
    //     } else if (tab->h)
    //         printf("0\n");
    // }
    // #endif
    tab->n_line = 0;
    tab->c_line = 0;
}

void cicle_while(int* flag_l, FILE* fd, parser_grep* tab, int* count_file, char* name_file) {
    regex_t reg;
    regmatch_t rmt[1];
    char* buf = NULL;
    size_t n = 0;
    int stop = 0;
    while (getline(&buf, &n, fd) != EOF && stop == 0) {
        tab->n_line++;
        int error = 0;
        int _reg_ = 0;
        if (tab->i) {
            _reg_ = REG_ICASE;
        } else {
            _reg_ = REG_EXTENDED;
        }
        size_t n_v = 0;
        for (size_t j = 0; j < tab->count; j++) {
            regcomp(&reg, tab->pattern[j], _reg_);
            error = regexec(&reg, buf, 1, rmt, 0);
            if (error == 1 && tab->v && strlen(tab->pattern[j]) != 0) {
                n_v++;
            }
            if (error == 0 && !tab->v) {
                *flag_l = 1;
                tab->c_line++;
                if (!tab->l && !tab->c)
                    print_reg(buf, *count_file, name_file, tab);
                regfree(&reg);
                break;
            }
            regfree(&reg);
            if (n_v == tab->count && tab->v) {
                *flag_l = 1;
                if (!tab->c && !tab->l) {
                    print_reg(buf, *count_file, name_file, tab);
                } else if (tab->v) {
                    tab->c_line++;
                }
            }
        }
        if (tab->l && *flag_l > 0 && !tab->c)
            stop = 1;
    }
    free(buf);
}

void print_reg(char* buf, int counter_files, char* argv, parser_grep* tab) {
    if (counter_files > 1 && !tab->h)
        printf("%s:", argv);
    if (tab->n)
        printf("%ld:", tab->n_line);
    if (buf[strlen(buf) - 1] != '\n') {
        printf("%s\n", buf);
    } else {
        printf("%s", buf);
    }
}

void pattern_init_without_f_e(int argc, char** argv, parser_grep* tab) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            tab->pattern[0] = argv[i];
            tab->count++;
            break;
        }
    }
}

void grep_null_flag(int argc, char** argv, parser_grep* tab, int counter_files) {
    regex_t reg;
    regmatch_t rmt[1];
    for (int i = 2; i < argc; i++) {
        FILE* fd;
        fd = fopen(argv[i], "r");
        if (fd) {
            char* buf = NULL;
            size_t n = 0;
            while (!feof(fd)) {
            getline(&buf, &n, fd);
                if (buf != NULL) {
                    int error = 0;
                    for (size_t j = 0; j < 1; j++) {
                        regcomp(&reg, tab->pattern[j], REG_EXTENDED);
                        error = regexec(&reg, buf, 1, rmt, 0);
                        if (error == 0) {
                            if (counter_files > 1) {
                                printf("%s:%s", argv[i], buf);
                            } else {
                                printf("%s", buf);
                            }
                            regfree(&reg);
                            break;
                        }
                        regfree(&reg);
                    }
                }
            }
            free(buf);
            fclose(fd);
        } else {
            if (!tab->s)
                fprintf(stderr, "s21_grep: %s: no such file or directory\n", *(argv + i));
        }
    }
}

int file_checker(int argc, char** argv, parser_grep* tab) {
    int checker = 0;
    int check_pat = 0;
    if (!tab->f && !tab->e) {
        for (size_t i = 1; (int)i < argc; i++) {
            if (argv[i - 1][0] == '-' && !tab->e_check && (strstr(argv[i - 1], "f") != NULL
                || strstr(argv[i - 1], "e") != NULL))
                i++;
            if ((int)i >= argc)
                break;
            if (argv[i][0] != '-') {
                check_pat++;
                if (check_pat > 1)
                    checker++;
            }
        }
    } else {
        for (size_t i = 1; (int)i < argc; i++) {
            if (argv[i - 1][0] == '-' && !tab->e_check && (strstr(argv[i - 1], "f") != NULL
                || strstr(argv[i - 1], "e") != NULL))
                i++;
            if ((int)i >= argc)
                break;
            if (argv[i][0] != '-') {
                checker++;
            }
        }
    }
    return checker;
}

int init_flags(int argc, char** argv, parser_grep* tab) {
    int res = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (size_t j = 1; j != 0 && argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'c':  // counter strings
                        tab->c = 1;
                        res++;
                        break;
                    case 'e':
                        if (argv[i][j + 1] == '\0' && argv[i + 1] != NULL) {
                            if (argv[i + 1][0] == '-') {
                                i++;
                                init_e(argv[i], tab);
                                j = 0;
                            } else {
                                init_e(argv[i + 1], tab);
                            }
                            res++;
                        } else if (argv[i][j] != '\0') {
                            char* eflag = strstr(argv[i], "e");
                            eflag++;
                            init_e(eflag, tab);
                            res++;
                            j = 0;
                            tab->e_check = 1;
                        } else {
                            if (!tab->s)
                                printf("Null argiment after flag 'e'\n");
                            tab->error = -1;
                        }
                        break;
                    case 'f':
                        if (argv[i + 1] != NULL) {
                            init_f(argv[i + 1], tab);
                            res++;
                        } else {
                            if (!tab->s)
                                printf("Null argiment after flag 'f'\n");
                            tab->error = -1;
                        }
                        break;
                    case 'h':
                        tab->h = 1;
                        res++;
                        break;
                    case 'i':
                        tab->i = 1;
                        res++;
                        break;
                    case 'l':
                        tab->l = 1;
                        res++;
                        break;
                    case 'n':
                        tab->n = 1;
                        res++;
                        break;
                    case 'o':
                        tab->o = 1;
                        res++;
                        break;
                    case 's':
                        res++;
                        tab->s = 1;
                        break;
                    case 'v':
                        tab->v = 1;
                        res++;
                        break;
                    default:
                        if (!tab->s)
                            printf("Invalid argument: %c\n", argv[i][j]);
                        tab->error = -1;
                        break;
                }
                if (j == 0)
                    break;
            }
        }
        if (i == argc)
            break;
    }
    return res;
}

void init_f(char* file_name, parser_grep* tab) {
    FILE* fd;
    fd = fopen(file_name, "r");
    if (fd) {
        char* buf = NULL;
        tab->f = 1;
        size_t n = 0;
        while (getline(&buf, &n, fd) != EOF) {
            if (buf != NULL) {
                tab->pattern = realloc(tab->pattern, (tab->count + 1) * sizeof(tab->pattern));
                tab->pattern[tab->count] = (char*)calloc(strlen(buf), sizeof(char));
                strncpy(tab->pattern[tab->count], buf, strlen(buf) - 1);
                tab->pattern[tab->count][strcspn(tab->pattern[tab->count], "\r\n")] = '\0';
                if (!strlen(tab->pattern[tab->count])) {
                    tab->pattern[tab->count][0] = '.';
                }
                tab->count++;
            }
        }
        free(buf);
        fclose(fd);
    } else {
        tab->error = -1;
        if (!tab->s)
            printf("after flag 'f', %s - isn't file\n", file_name);
    }
}

void init_e(char* buf, parser_grep* tab) {
    tab->e = 1;
    tab->pattern = realloc(tab->pattern, (tab->count + 1) * sizeof(tab->pattern));
    tab->pattern[tab->count] = (char*)calloc(strlen(buf) + 1, sizeof(char));
    strcpy(tab->pattern[tab->count], buf);
    tab->count++;
}

parser_grep* tab_null(parser_grep* tab) {
    tab->c = 0;
    tab->e = 0;
    tab->f = 0;
    tab->h = 0;
    tab->i = 0;
    tab->l = 0;
    tab->n = 0;
    tab->o = 0;
    tab->s = 0;
    tab->v = 0;
    tab->pattern = NULL;
    tab->error = 0;
    tab->str_counter = 0;
    tab->n = 0;
    tab->c_line = 0;
    tab->flags_res = 0;
    tab->n_line = 0;
    tab->e_check = 0;
    tab->fl_count = 0;
    return tab;
}
