#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

Parser* create_parser(const char* filename){
    Parser* p = malloc(sizeof(Parser));
    p->fp = fopen(filename, "r");

    if (!p->fp){
        printf("Error opening file %s \n", filename);
        exit(1);
    }
    p->current[0] = '\0';
    return p;
}

void destroy_parser(Parser* p){
    fclose(p->fp);
    free(p);
}

int is_space(char s){
    return s == ' ';
}

static void strip(char* s){
    char* comment = strstr(s, "//"); //points to substring starting first /
    if (comment){
        *comment = '\0';    //null character the comment
    }

    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
        len--;
    }

    for (int i = strlen(s)-1; i>=0 && is_space(s[i]); i--){
        s[i] = '\0';
    }
}

int parser_hasMoreCommands(Parser* p){
    if (!feof(p->fp)){
        return 1;
    }
    return 0;
}

int parser_advance(Parser* p){
    while (fgets(p->current, sizeof(p->current), p->fp)){
        strip(p->current);
        if (p->current[0] != '\0')
            return 1;
    }
    p->current[0] = '\0';
    return 0;
}

C_t parser_commandType(Parser* p){
    if (strncmp(p->current, "push", 4) == 0){
        return C_PUSH;
    }else if (strncmp(p->current, "pop", 3) == 0){
        return C_POP;
    }else if (strncmp(p->current, "label", 5) == 0){
        return C_LABEL;
    }else if (strncmp(p->current, "goto", 4) == 0){
        return C_GOTO;
    }else if (strncmp(p->current, "if-goto", 7) == 0){
        return C_IF;
    }else if (strncmp(p->current, "function", 8) == 0){
        return C_FUNCTION;
    }else if (strncmp(p->current, "call", 4) == 0){
        return C_CALL;
    }else if (strncmp(p->current, "return", 6) == 0){
        return C_RETURN;
    }else{
        return C_ARITHMETIC;
    }
}

char* get_token(char* s, int target_index) {
    int current = 0;
    while (*s == ' ') s++;

    while (*s) {
        char* start = s;
        char buf[100];
        size_t i = 0;
        while (*s && *s != ' ' && i < sizeof(buf)-1) {
            buf[i++] = *s++;
        }
        buf[i] = '\0';

        if (current == target_index) {
            char* copy = malloc(i+1);
            strcpy(copy, buf);
            return copy; // caller must free()
        }

        current++;
        while (*s == ' ') s++;
    }
    return NULL;
}

char* parser_cmd(Parser* p){
    return get_token(p->current, 0);
}

char* parser_arg1(Parser* p){
    return get_token(p->current, 1);
}

int parser_arg2(Parser* p){
    char* tok = get_token(p->current, 2);
    int value = atoi(tok);
    free(tok);
    return value;
}

