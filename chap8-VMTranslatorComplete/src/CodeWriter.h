#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <stdio.h>

typedef struct {
    FILE* out;
    char fileName[256];
    char currentFunction[256]; //keep the current function in here
    int labelCounter;
} CodeWriter;

void create_codewriter(CodeWriter* cw, const char* outputFile);
void codewriter_setFileName(CodeWriter* cw, const char* fileName);
void codewriter_writeInit(CodeWriter* cw);
void codewriter_writeArithmetic(CodeWriter* cw, const char* command);
void codewriter_writePushPop(CodeWriter* cw, int commandType, const char* segment, int index);
void codewriter_writeLabel(CodeWriter* cw, char* label);
void codewriter_writeGoto(CodeWriter* cw, char* label);
void codewriter_writeIf(CodeWriter* cw, char* label);
void codewriter_writeFunction(CodeWriter* cw, const char* functionName, int numLocals);
void codewriter_writeCall(CodeWriter* cw, const char* functionName, int nArgs);
void codewriter_writeReturn(CodeWriter* cw);
void codewriter_close(CodeWriter* cw);

#endif
