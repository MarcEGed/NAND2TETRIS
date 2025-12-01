#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <stdio.h>

typedef struct {
    FILE* out;
    char fileName[256];
    int labelCounter;
} CodeWriter;

void create_codewriter(CodeWriter* cw, const char* outputFile);
void codewriter_setFileName(CodeWriter* cw, const char* fileName);
void codewriter_writeArithmetic(CodeWriter* cw, const char* command);
void codewriter_writePushPop(CodeWriter* cw, int commandType, const char* segment, int index);
void codewriter_close(CodeWriter* cw);

#endif
