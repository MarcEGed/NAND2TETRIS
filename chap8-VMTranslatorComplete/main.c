#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "src/parser.h"
#include "src/CodeWriter.h"

//compile a single VM file
//writeInit: whether to call codewriter_writeInit (only for Sys.vm)
void compile_file(const char* file, CodeWriter* cw, int writeInit) {
    printf("[INFO] Compiling: %s\n", file);

    Parser* p = create_parser(file);

    if (writeInit) {
        printf("[INFO] Writing initialization for Sys.vm\n");
        codewriter_writeInit(cw);
    }

    codewriter_setFileName(cw, file);

    while (parser_advance(p)) {
        C_t type = parser_commandType(p);

        switch (type) {
            case C_ARITHMETIC: {
                char* cmd = parser_cmd(p);
                codewriter_writeArithmetic(cw, cmd);
                free(cmd);
                break;
            }

            case C_PUSH:
            case C_POP: {
                char* segment = parser_arg1(p);
                int index = parser_arg2(p);

                //minimal debug: only show push/pop commands
                printf("[INFO] %s %s %d\n", type == C_PUSH ? "push" : "pop", segment, index);

                codewriter_writePushPop(cw, type == C_POP, segment, index);
                free(segment);
                break;
            }

            case C_LABEL: {
                char* label = parser_arg1(p);
                codewriter_writeLabel(cw, label);
                free(label);
                break;
            }

            case C_GOTO: {
                char* label = parser_arg1(p);
                codewriter_writeGoto(cw, label);
                free(label);
                break;
            }

            case C_IF: {
                char* label = parser_arg1(p);
                codewriter_writeIf(cw, label);
                free(label);
                break;
            }

            case C_FUNCTION: {
                char* funcName = parser_arg1(p);
                int nLocals = parser_arg2(p);
                codewriter_writeFunction(cw, funcName, nLocals);
                strcpy(cw->currentFunction, funcName);
                free(funcName);
                break;
            }

            case C_CALL: {
                char* funcName = parser_arg1(p);
                int nArgs = parser_arg2(p);
                codewriter_writeCall(cw, funcName, nArgs);
                free(funcName);
                break;
            }

            case C_RETURN: {
                codewriter_writeReturn(cw);
                break;
            }
        }
    }

    destroy_parser(p);
}

//main program
int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  %s -f <file.vm>  # single file mode\n", argv[0]);
        printf("  %s -d  <folder>   # compile all .vm in folder\n", argv[0]);
        return 1;
    }

    CodeWriter cw;
    char output[512];

    //single-file mode
    if (strcmp(argv[1], "-f") == 0) {
        const char* input = argv[2];

        //generate output filename automatically
        strcpy(output, input);
        char* dot = strrchr(output, '.');
        if (dot) *dot = '\0';
        strcat(output, ".asm");

        printf("[INFO] Output file: %s\n", output);

        create_codewriter(&cw, output);
        compile_file(input, &cw, 1); //no init for single files
        codewriter_close(&cw);
    } 
    //directory mode
    else if (strcmp(argv[1], "-d") == 0) {
        const char* folder = argv[2];

        //output filename = folder.asm
        const char* lastSlash = strrchr(folder, '/');
        const char* folderName = lastSlash ? lastSlash + 1 : folder;
        sprintf(output, "%s.asm", folderName);
        printf("[INFO] Output file: %s\n", output);

        create_codewriter(&cw, output);

        DIR* dir = opendir(folder);
        if (!dir) {
            printf("Error: cannot open directory %s\n", folder);
            return 1;
        }

        struct dirent* entry;

        //first compile Sys.vm if it exists
        rewinddir(dir);
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, "sys.vm") == 0) {
                char path[512];
                sprintf(path, "%s/%s", folder, entry->d_name);
                compile_file(path, &cw, 1); // writeInit
            }
        }

        //then compile all other .vm files
        rewinddir(dir);
        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, ".vm") && strcmp(entry->d_name, "Sys.vm") != 0) {
                char path[512];
                sprintf(path, "%s/%s", folder, entry->d_name);
                compile_file(path, &cw, 0); //no init
            }
        }

        closedir(dir);
        codewriter_close(&cw);
    } 
    else {
        printf("Unknown flag: %s\n", argv[1]);
        return 1;
    }

    printf("[INFO] Compilation finished.\n");
    return 0;
}