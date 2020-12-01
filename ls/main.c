#include <pwd.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

typedef struct stat file_stat;
typedef struct dirent dir_stat;
typedef struct program_info {
    char * dir_name;
    char flag;
} program_argc;

program_argc * init (int, char * []);                   //Собираем аргументы командной строки и сохраняем
void start (program_argc *);                            //Запускает нужную функцию в зависимости от флага
void be_lazy (program_argc *);                          //Главный процесс ждет завершения дочернего
void look_through (char *, int);                        //Древовидный рекурсивный обход каталога
void look (char *);                                     //Информация о файлах текущего каталога: ls -l
void view_file_full (char *, file_stat *);              //Вывод на экран полной информации конкретного файла
void view_file_short (char *, file_stat *);             //Вывод на экран базовой информации конкретного файла
char * make_full_path (char *, char *);         //Склеивание полного пути для файла

int main (int argc, char * argv[]) {
    program_argc * info = init(argc, argv);

    pid_t pid;
    switch (pid = fork()) {
        case 0:
            start(info);
            break;
        case -1:
            perror("Cannot create a child :(");
            exit(EXIT_FAILURE);
            break;
        default:
            be_lazy(info);
    }
    return EXIT_SUCCESS;
}

program_argc * init (int argc, char * argv[]) {
    program_argc * info = (program_argc *)calloc(1, sizeof(program_argc));
    info->dir_name = ".";
    switch (argc) {
        case 1:
            //Если нет дополнительных аргументов
            break;
        case 2:
            info->dir_name = argv[1];
            break;
        case 3:
            info->dir_name = argv[1];
            info->flag = *argv[2];
            break;
        default:
            printf("Wrong args!\n");
            exit(EXIT_FAILURE);
    }
    return info;
}

void start (program_argc * info) {
    printf("\n");
    switch (info->flag) {
        case 'r':
            look_through(info->dir_name, 0);
            break;
        default:
            look(info->dir_name);
            break;
    }
    free(info);
}

void be_lazy (program_argc * info) {
    int status;
    wait(&status);
    if (status == EXIT_SUCCESS)
        printf("\nWell Done!\n");
    else
        printf("\nThere is something wrong!\n");
    free(info);
    exit(status);
}

void look_through (char * dir_name, int level) {
    DIR * dir;
    dir_stat entry;
    dir_stat * entry_ptr = NULL;
    file_stat * info = (file_stat *)calloc(1, sizeof(file_stat));

    if ((dir = opendir(dir_name))) {
        readdir_r(dir, &entry, &entry_ptr);
        while (entry_ptr != NULL) {
            if (entry.d_type == DT_DIR) {
                if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, "..")) {
                    readdir_r(dir, &entry, &entry_ptr);
                    continue;
                } else {
                    char * path = make_full_path(dir_name, entry.d_name);
                    if (level) printf("%*s", level, " ");
                    if (stat(path, info)) {
                        perror("Cannot open file");
                    } else {
                        view_file_short(entry.d_name, info);
                        look_through(path, level + 4);
                    }
                    readdir_r(dir, &entry, &entry_ptr);
                    free(path);
                }
            } else {
                char * path = make_full_path(dir_name, entry.d_name);
                if (level) printf("%*s", level, " ");
                if (stat(path, info)) {
                    perror("Cannot open file");
                    free(path);
                } else {
                    view_file_short(entry.d_name, info);
                }
                readdir_r(dir, &entry, &entry_ptr);
                free(path);
            }
        }
        fflush(stdout);
        closedir(dir);
        free(entry_ptr);
        free(info);
    } else {
        perror("Cannot open dir");
        return;
    }
}

void look (char * dir_name) {
    DIR * dir;
    dir_stat entry;
    dir_stat * entry_ptr = NULL;
    file_stat * info = (file_stat *)calloc(1, sizeof(file_stat));

    if ((dir = opendir(dir_name))) {
        readdir_r(dir, &entry, &entry_ptr);
        while (entry_ptr != NULL) {
            if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, "..")) {
                readdir_r(dir, &entry, &entry_ptr);
                continue;
            }
            char * path = make_full_path(dir_name, entry.d_name);
            if (stat(path, info)) {
                perror("Cannot read file");
            } else {
                view_file_full(entry.d_name, info);
            }
            readdir_r(dir, &entry, &entry_ptr);
            free(path);
        }
        fflush(stdout);
        closedir(dir);
        free(entry_ptr);
        free(info);
    } else {
        perror("Cannot open dir");
        return;
    }
}

void view_file_full (char * file_name, file_stat * info) {

    struct tm timer = *localtime(&info->st_atime);
    struct passwd * owner = getpwuid(info->st_uid);

    char is_dir = (S_ISDIR(info->st_mode)) ? 'd' : '-';
    char usr_r = (S_IRUSR & info->st_mode) ? 'r' : '-';
    char usr_w = (S_IWUSR & info->st_mode) ? 'w' : '-';
    char usr_x = (S_IXUSR & info->st_mode) ? 'x' : '-';
    char grp_r = (S_IRGRP & info->st_mode) ? 'r' : '-';
    char grp_w = (S_IWGRP & info->st_mode) ? 'w' : '-';
    char grp_x = (S_IXGRP & info->st_mode) ? 'x' : '-';
    char oth_r = (S_IROTH & info->st_mode) ? 'r' : '-';
    char oth_w = (S_IWOTH & info->st_mode) ? 'w' : '-';
    char oth_x = (S_IXOTH & info->st_mode) ? 'x' : '-';

    printf("%c%c%c%c%c%c%c%c%c%c\t%-10s", is_dir, usr_r, usr_w, usr_x, grp_r, grp_w, grp_x, oth_r, oth_w, oth_x, owner->pw_name);
    printf("%-15s%-10lld%.2d:%.2d:%.2d\n", file_name, (long long)info->st_size, timer.tm_hour, timer.tm_min, timer.tm_sec);
}

void view_file_short (char * file_name, file_stat * info) {
    printf("%s - %lld\n", file_name, (long long)info->st_size);
}

char * make_full_path (char * parent_dir, char * dir) {
    char * full_path;
    full_path = (char *)calloc(strlen(parent_dir) + strlen(dir) + 2, sizeof(char));
    strcpy(full_path, parent_dir);
    strcat(full_path, "/");
    strcat(full_path, dir);
    return full_path;
}
