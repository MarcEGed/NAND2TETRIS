#include "src/parser.h"
#include "src/CodeWriter.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s input.vm output.vm\n", argv[0]);
        return 1;
    }

    const char* input = argv[1];
    const char* out   = argv[2];
    printf("%s %s \n", input, out);

    Parser* p = create_parser(argv[1]);

    CodeWriter cw;
    create_codewriter(&cw, out);
    codewriter_setFileName(&cw, input);
    codewriter_writeInit(&cw);
    cw.currentFunction[0] = '\0';

    while (parser_advance(p)){
        C_t type = parser_commandType(p);

        if (type == C_ARITHMETIC){
            char* cmd = parser_cmd(p);
            codewriter_writeArithmetic(&cw, cmd);
            free(cmd);
        }
        else if (type == C_PUSH || type == C_POP){
            char* segment = parser_arg1(p);
            int index = parser_arg2(p);
            printf("DEBUG: segment = '%s'\n", segment);
            printf("DEBUG: index = %d\n",index);
            if (type == C_PUSH) codewriter_writePushPop(&cw, 0, segment, index);
            if (type == C_POP)  codewriter_writePushPop(&cw, 1, segment, index);
            
            free(segment);
        }else if (type == C_LABEL){
            char* labelName = parser_arg1(p);
            codewriter_writeLabel(&cw, labelName);
            free(labelName);
        }else if (type == C_GOTO){
            char* labelName = parser_arg1(p);
            codewriter_writeGoto(&cw, labelName);
            free(labelName);
        }else if (type == C_IF){
            char* labelName = parser_arg1(p);
            codewriter_writeIf(&cw, labelName);
            free(labelName);
        }else if (type == C_FUNCTION) {
            char* funcName = parser_arg1(p);
            int nLocals = parser_arg2(p);
            codewriter_writeFunction(&cw, funcName, nLocals);
            strcpy(cw.currentFunction, funcName); // track current function
            free(funcName);
        }else if (type == C_CALL) {
            char* funcName = parser_arg1(p);
            int nArgs = parser_arg2(p);
            codewriter_writeCall(&cw, funcName, nArgs);
            free(funcName);
        }else if (type == C_RETURN) {
            codewriter_writeReturn(&cw);
        }

    }
    
    destroy_parser(p);
    codewriter_close(&cw);

    return 0;
}