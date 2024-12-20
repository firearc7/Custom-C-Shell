#include "reveal.h"

int compare_strings(const void* a, const void* b){
    const char* str_a = *(const char**)a;
    const char* str_b = *(const char**)b;
    return strcmp(str_a, str_b);
}

int flags(char* arg){
    int a= 0;
    int l= 0;
    while (*arg!='\0'){
        if (*arg== 'a'){
            a= 1;
        }
        else if (*arg== 'l'){
            l= 1;
        }
        else{
            return -1;
        }
        arg++;
    }
    if (a && l){
        return 3;
    }
    else if (a){
        return 1;
    }
    else if (l){
        return 2;
    }
    return 0;
}

void print_file_info(struct stat* file_stat, char* file_name){
    printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");

    printf("%ld ", file_stat->st_nlink);

    struct passwd* pwd= getpwuid(file_stat->st_uid);
    struct group* grp= getgrgid(file_stat->st_gid);
    printf("%s %s ", pwd->pw_name, grp->gr_name);

    printf("%5ld ", file_stat->st_size);

    char* time= ctime(&file_stat->st_mtime);
    time[strlen(time)-1]= '\0';
    printf("%s ", time);

    if (S_ISDIR(file_stat->st_mode)){
        printf(BLUE "%s" WHITE, file_name);
    }
    else if (file_stat->st_mode & S_IXUSR){
        printf(GREEN "%s" WHITE, file_name);
    }
    else{
        printf("%s", file_name);
    }
    printf("\n");
}

void reveal(char** args, char* home, char* pwd, char* cwd){
    int show_all= 0;
    int show_long= 0;
    int i= 1;
    char* path= (char*) malloc(BUFSIZE* sizeof(char));
    if (path==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return;
    }
    strcpy(path, ".");
    while (args[i]!=NULL){
        if (args[i][0]=='-' && strlen(args[i])>1){
            int f= flags(args[i]+ 1);
            if (f==-1 || f==0){
                fprintf(stderr, RED);
                perror("Invalid flag");
                fprintf(stderr, WHITE);
                return;
            }
            if (f== 1){
                show_all= 1;
            }
            else if (f== 2){
                show_long= 1;
            }
            else if (f== 3){
                show_all= 1;
                show_long= 1;
            }
        }
        else if (args[i][0]=='-' && strlen(args[i])==1){
            strcpy(path, pwd);
            strcat(path, "/");
            if (args[i+1]!=NULL){
                fprintf(stderr, RED);
                perror("Invalid path");
                fprintf(stderr, WHITE);
            }
            break;
        }
        else{
            if (args[i][0]=='~'){
                strcpy(path, home);
                if (strlen(args[i])>1){
                    strcat(path, args[i]+1);
                }
            }
            else{
                strcpy(path, args[i]);
            }

            if (args[i+1]!=NULL){
                fprintf(stderr, RED);
                perror("Invalid path");
                fprintf(stderr, WHITE);
            }
            break;
        }
        i++;
    }

    struct stat file_stat_filecheck;
    if (stat(path, &file_stat_filecheck)==-1){
        fprintf(stderr, RED);
        perror("stat() error");
        fprintf(stderr, WHITE);
        return;
    }
    if (S_ISREG(file_stat_filecheck.st_mode)){
        if (show_long){
            print_file_info(&file_stat_filecheck, path);
        }
        else{
            printf("%s\n", path);
        }
        return;
    }

    DIR* dir= opendir(path);
    if (dir==NULL){
        fprintf(stderr, RED);
        perror("opendir() error");
        fprintf(stderr, WHITE);
        return;
    }

    struct dirent* entry;
    struct stat file_stat;
    char full_path[BUFSIZE];
    char* files[BUFSIZE];
    int file_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.'){
            continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &file_stat) == -1) {
            perror("stat() error");
            continue;
        }
        files[file_count] = strdup(entry->d_name);
        file_count++;
    }
    closedir(dir);

    qsort(files, file_count, sizeof(char*), compare_strings);

    for (int i = 0; i < file_count; i++) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, files[i]);
        if (stat(full_path, &file_stat) == -1){
            fprintf(stderr, RED);
            perror("stat() error");
            fprintf(stderr, WHITE);
            free(files[i]);
            continue;
        }
        if (show_long){
            print_file_info(&file_stat, files[i]);
        }
        else{
            if (S_ISDIR(file_stat.st_mode)){
                printf(BLUE "%s" WHITE "\n", files[i]);
            }
            else if (file_stat.st_mode & S_IXUSR){
                printf(GREEN "%s" WHITE "\n", files[i]);
            }
            else{
                printf("%s\n", files[i]);
            }
        }
        free(files[i]);
    }
    free(path);
}