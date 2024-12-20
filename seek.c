#include "seek.h"

char* handle_e_flag= NULL;
bool e_global= false;
int count= 0;

int flags2(char* arg){
    int flags= 0;
    while (*arg!='\0'){
        switch (*arg){
            case 'd':
                flags|= 4;
                break;
            case 'e':
                flags|= 2;
                break;
            case 'f':
                flags|= 1;
                break;
            default:
                return -1;
        }
        arg++;
    }
    return flags;
}

void walk(char* path, char* target, int show_d, int show_f){
    DIR* dir= opendir(path);
    if (dir==NULL){
        fprintf(stderr, RED);
        perror("opendir() error");
        fprintf(stderr, WHITE);
        return;
    }
    struct dirent* entry;
    while ((entry=readdir(dir))!=NULL){
        if (strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){
            continue;
        }
        char fullPath[512];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        struct stat path_stat;
        stat(fullPath, &path_stat);

        if (S_ISDIR(path_stat.st_mode)){
            if (show_d && strstr(entry->d_name, target) != NULL){
                count++;
                if(e_global){
                    if (handle_e_flag==NULL){
                        handle_e_flag= (char*) malloc(BUFSIZE* sizeof(char));
                        if (handle_e_flag==NULL){
                            fprintf(stderr, RED);
                            perror("malloc() error");
                            fprintf(stderr, WHITE);
                            closedir(dir);
                            return;
                        }
                        strcpy(handle_e_flag, fullPath);
                    }
                    else{
                        e_global= false;
                        printf(BLUE "./%s\n" WHITE, handle_e_flag+strlen(path)+1);
                        free(handle_e_flag);
                        handle_e_flag= NULL;
                        printf(BLUE "./%s\n" WHITE, fullPath+strlen(path)+1);
                    }
                }
                else{
                    printf(BLUE "./%s\n" WHITE, fullPath+strlen(path)+1);
                }
            }
            walk(fullPath, target, show_d, show_f);
        }
        else if (S_ISREG(path_stat.st_mode)){
            if (show_f && strstr(entry->d_name, target) != NULL){
                count++;
                if(e_global){
                    if (handle_e_flag==NULL){
                        handle_e_flag= (char*) malloc(BUFSIZE* sizeof(char));
                        if (handle_e_flag==NULL){
                            fprintf(stderr, RED);
                            perror("malloc() error");
                            fprintf(stderr, WHITE);
                            closedir(dir);
                            return;
                        }
                        strcpy(handle_e_flag, fullPath);
                    }
                    else{
                        e_global= false;
                        printf(GREEN "./%s\n" WHITE, handle_e_flag+strlen(path)+1);
                        free(handle_e_flag);
                        handle_e_flag= NULL;
                        printf(GREEN "./%s\n" WHITE, fullPath+strlen(path)+1);
                    }
                }
                else{
                    printf(GREEN "./%s\n" WHITE, fullPath+strlen(path)+1);
                }
            }
        }
    }
    closedir(dir);
}

void seek_cmd(char** args, char* home, char* pwd, char* cwd){
    count= 0;
    if (handle_e_flag!=NULL){
        free(handle_e_flag);
        handle_e_flag= NULL;
    }
    int show_d= 0;
    int show_e= 0;
    int show_f= 0;
    int i= 1;
    if (args[i]==NULL){
        fprintf(stderr, RED);
        perror("No path provided");
        fprintf(stderr, WHITE);
        return;
    }
    char* target= (char*) malloc(BUFSIZE*sizeof(char));
    char* path= (char*) malloc(BUFSIZE*sizeof(char));
    if (path==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return;
    }
    if (target==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return;
    }
    strcpy(path, cwd);
    bool flags_exist= false;
    while (args[i]!=NULL){
        if (args[i][0]=='-' && strlen(args[i])>1){
            flags_exist= true;
            int f= flags2(args[i]+ 1);
            if (f==-1){
                fprintf(stderr, RED);
                perror("Invalid flag");
                fprintf(stderr, WHITE);
                free(path);
                free(target);
                return;
            }
            if (f==4){
                show_d= 1;
            }
            else if (f==2){
                show_e= 1;
            }
            else if (f==1){
                show_f= 1;
            }
            else if (f==6){
                show_d= 1;
                show_e= 1;
            }
            else if (f==3){
                show_e= 1;
                show_f= 1;
            }
            else{
                fprintf(stderr, RED);
                perror("Invalid flag combination");
                fprintf(stderr, WHITE);
                free(path);
                free(target);
                return;
            }
        }
        else{
            strcpy(target, args[i]);
            if (args[i+1]==NULL){
                strcpy(path, cwd);
            }
            else if (args[i+2]!=NULL){
                fprintf(stderr, RED);
                perror("Invalid path");
                fprintf(stderr, WHITE);
                return;
            }
            else{
                if (args[i+1][0]=='-' && strlen(args[i])==1){
                    strcpy(path, pwd);
                }
                else{
                    if (args[i+1][0]=='~'){
                        strcpy(path, home);
                        if (strlen(args[i+1])>1){
                            strcat(path, args[i+1]+1);
                        }
                    }
                    else{
                        strcpy(path, args[i+1]);
                    }
                }   
            break;
            }
        }
        i++;
    }

    if (show_d==1 && show_f==1){
        fprintf(stderr, RED);
        perror("Invalid flag combination");
        fprintf(stderr, WHITE);
        free(path);
        free(target);
        return;
    }
    if (!flags_exist){
        show_d= 1;
        show_f= 1;
    }
    if (show_d==0 && show_f==0){
        show_d= 1;
        show_f= 1;
    }

    e_global= show_e;
    walk(path, target, show_d, show_f);

    if (count==0){
        fprintf(stderr, RED);
        perror("No match found!");
        fprintf(stderr, WHITE);
    }
    if (e_global){
        struct stat handle_e_flag_stat;
        if (stat(handle_e_flag, &handle_e_flag_stat)==-1){
            fprintf(stderr, RED);
            perror("stat() error");
            fprintf(stderr, WHITE);
            free(path);
            free(target);
            return;
        }
        else if (S_ISDIR(handle_e_flag_stat.st_mode)){
            hop(cwd, pwd, home, handle_e_flag);
        }
        else if (S_ISREG(handle_e_flag_stat.st_mode)){
            FILE* file= fopen(handle_e_flag, "r");
            if (file==NULL){
                fprintf(stderr, RED);
                perror("fopen() error");
                fprintf(stderr, WHITE);
                free(path);
                free(target);
                return;
            }
            char line[BUFSIZE];
            while (fgets(line, sizeof(line), file)){
                printf("%s", line);
            }
            fclose(file);
        }
    }
    free(path);
    free(target);
    return;
}