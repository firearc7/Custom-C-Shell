#include "input.h"

char* process_name;
double process_time;
extern int bg_process_count;

alias_info aliases= NULL;
func_info funct= NULL;

int main(){
    sigchild_handler(SIGCHLD);
    aliases= (alias_info) malloc(BUFSIZE*sizeof(alias));
    if (aliases==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return 1;
    }
    funct= (func_info) malloc(BUFSIZE*sizeof(func));
    if (funct==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return 1;
    }
    for (int i= 0; i<BUFSIZE; i++){
        aliases[i].init= NULL;
        aliases[i].fin= NULL;
    }
    for (int i= 0; i<BUFSIZE; i++){
        funct[i].init= NULL;
        funct[i].fin1= NULL;
        funct[i].fin2= NULL;
    }
    FILE* myshrc= fopen(".myshrc", "r");
    if (myshrc!=NULL){
        char* line= NULL;
        size_t len= 0;
        ssize_t read;
        int i= 0;
        int j= 0;
        while ((read= getline(&line, &len, myshrc))!=-1){
            line[strcspn(line, "\n")]= '\0';
            if (strlen(line)==0){
                continue;
            }
            char* token;
            if (strncmp(line, "func", 4)==0){
                char* func_line= strdup(line);
                token= strtok(func_line, " ");
                token= strtok(NULL, " ");
                token= strip(token);
                for (int k= 0; k<strlen(token); k++){
                    if (token[k]=='('){
                        token[k]='\0';
                    }
                }
                funct[j].init= strdup(token);
                free(func_line);
                read= getline(&line, &len, myshrc);
                read= getline(&line, &len, myshrc);
                token= strtok(line, " ");
                char* temp= strdup(token);
                temp= strip(temp);
                read= getline(&line, &len, myshrc);
                token= strtok(line, " ");
                char* temp2= strdup(token);
                temp2= strip(temp2);
                funct[j].fin1= strdup(temp);
                funct[j].fin2= strdup(temp2);
                j++;
                free(temp);
                free(temp2);
                read= getline(&line, &len, myshrc);
            }
            else{
                char* token= strtok(line, "=");
                aliases[i].init= strdup(token);
                aliases[i].init= strip(aliases[i].init);
                token= strtok(NULL, "=");
                aliases[i].fin= strdup(token);
                aliases[i].fin= strip(aliases[i].fin);
                i++;
            }
        }
        free(line);
        fclose(myshrc);
    }
    else{
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        return 1;
    }
    char home[BUFSIZE], cwd[BUFSIZE], pwd[BUFSIZE];
    process_name= (char*) malloc(BUFSIZE* sizeof(char));
    if (process_name==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return 1;
    }
    process_time= 0;
    if (getcwd(home, sizeof(home))==NULL){
        fprintf(stderr, RED);
        perror("getcwd() error");
        fprintf(stderr, WHITE);
        free(process_name);
        return 1;
    }
    if (getcwd(cwd, sizeof(cwd))==NULL){
        fprintf(stderr, RED);
        perror("getcwd() error");
        fprintf(stderr, WHITE);
        free(process_name);
        return 1;
    }
    if (getcwd(pwd, sizeof(pwd))==NULL){
        fprintf(stderr, RED);
        perror("getcwd() error");
        fprintf(stderr, WHITE);
        free(process_name);
        return 1;
    }
    char* input= (char*) malloc(BUFSIZE*sizeof(char));
    if (input==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        free(process_name);
        return 1;
    }
    char* home_copy= strdup(home);
    if (home_copy==NULL){
        fprintf(stderr, RED);
        perror("strdup() error");
        fprintf(stderr, WHITE);
        free(process_name);
        free(input);
        return 1;
    }
    signal(SIGCHLD, sigchild_handler);
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    while (true){
        display_prompt(home);
        if (fgets(input, BUFSIZE, stdin)==NULL){
            printf("\n");
            for (int i= 0; i<bg_process_count; i++){
                kill(bg_processes[i].pid, SIGKILL);
            }
            break;
        }
        input[strcspn(input, "\n")]= '\0';
        if (strlen(input)==0){
            continue;
        }
        add_to_log(input, home);
        split_command(input, home, cwd, pwd);
        strcpy(home, home_copy);
    }
    free(home_copy);
    free(input);
    free(process_name);
    return 0;
}