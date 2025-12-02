#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>

typedef enum {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
}C_t;

typedef struct{
    FILE* fp;
    char current[200];
}Parser;

Parser* create_parser(const char* filename);
void destroy_parser(Parser* p);

int parser_hasMoreCommands(Parser* p);
int parser_advance(Parser* p);
C_t parser_commandType(Parser* p);
char* parser_cmd(Parser* p);
char* parser_arg1(Parser* p);
int parser_arg2(Parser* p);

#endif