#include "helper.h"

char* strip(char* str){
    char* end;
    while (isspace((unsigned char)* str)){
        str++;
    }
    if (*str==0){
        return str;
    }
    end= str+ strlen(str)- 1;
    while (end>str && isspace((unsigned char)* end)){
        end--;
    }
    *(end+ 1)= '\0';
    return str;
}

bool in(char* small_str, char* big_str){
    if (strstr(big_str, small_str)!=NULL){
        return true;
    }
    return false;
}

void replace_tabs_with_spaces(char* str){
    for (int i = 0; str[i] != '\0'; i++){
        if (str[i]=='\t'){
            str[i]=' ';
        }
    }
    str= strip(str);
}

char* join(char** args){
    char* joined= (char*) malloc(BUFSIZE* sizeof(char));
    if (joined==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return NULL;
    }
    joined[0]= '\0';
    for (int i=0; args[i]!=NULL; i++){
        strcat(joined, args[i]);
        strcat(joined, " ");
    }
    return joined;
}

char* replace(char* str, char* old, char* new){
    char* result;
    int i, count= 0;
    int new_len= strlen(new);
    int old_len= strlen(old);
    for (i=0; str[i]!='\0'; i++){
        if (strstr(&str[i], old)==&str[i]){
            count++;
            i+= old_len- 1;
        }
    }
    result= (char*) malloc(i+ count* (new_len- old_len)+ 1);
    if (result==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return NULL;
    }
    i= 0;
    while (*str){
        if (strstr(str, old)==str){
            strcpy(&result[i], new);
            i+= new_len;
            str+= old_len;
        }
        else{
            result[i++]= *str++;
        }
    }
    result[i]= '\0';
    return result;
}