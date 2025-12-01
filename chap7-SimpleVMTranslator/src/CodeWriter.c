#include "CodeWriter.h"
#include <string.h>
#include <stdlib.h>

void create_codewriter(CodeWriter* cw, const char* outputFile) {
    cw->out = fopen(outputFile, "w");
    cw->labelCounter = 0;

    if (!cw->out) {
        printf("Error: cannot open output file.\n");
        exit(1);
    }
}

void codewriter_setFileName(CodeWriter* cw, const char* fileName) {
    strcpy(cw->fileName, fileName);
}

void pushDToStack(FILE* out) {
    fprintf(out,
        "@SP\n"             //load Stack Pointer into A  
        "A=M\n"             //A = RAM[SP]
        "M=D\n"             //RAM[A] = D, D is written to the top of the stack
        "@SP\n"             //load SP into A
        "M=M+1\n");         //increment SP to grow the stack
}

void popStackToD(FILE* out) {
    fprintf(out,
        "@SP\n"             //load SP into A
        "M=M-1\n"           //decrement SP
        "A=M\n"             //A = address of the top item
        "D=M\n");           //D = RAM[A]
}

const char* segmentBase(const char* segment) {
    if (strcmp(segment, "local") == 0) return "LCL";
    if (strcmp(segment, "argument") == 0) return "ARG";
    if (strcmp(segment, "this") == 0) return "THIS";
    if (strcmp(segment, "that") == 0) return "THAT";
    return NULL;
}

void computeSegmentAddress(FILE* out, const char* base, int value) {
    //used for LCL/ARG/THIS/THAT
    fprintf(out,
        "@%d\n"             //A = value    
        "D=A\n"             //D = A = value
        "@%s\n"             //A = base
        "D=M+D\n"           //D = RAM[A] + D
        "@R13\n"            //A = R13
        "M=D\n",            //M[R13] = D
        value, base);
}

void codewriter_writeArithmetic(CodeWriter* cw, const char* command) {
    FILE* out = cw->out;
    
    if (strcmp(command, "add") == 0){
        fprintf(out,
            "@SP\n"
            "M=M-1\n"   // SP--
            "A=M\n"
            "D=M\n"     // D = y (top of stack)
            "A=A-1\n"   // move to x
            "M=D+M\n"); // x = x + y
    }
    else if (strcmp(command, "neg") == 0){
        fprintf(out,
            "@SP\n"
            "A=M-1\n"
            "M=-M\n"
        );
    }
    else if (strcmp(command, "and") == 0){
        popStackToD(out); //D = y
        fprintf(out,
            "@SP\n"
            "A=M\n"
            "D=M&D\n"     //D = y&M
        );
        pushDToStack(out);
    }
    else if (strcmp(command, "or") == 0) {
        popStackToD(out);
        fprintf(out,
            "@SP\n"
            "A=M\n"
            "D=M|D\n"
        );
        pushDToStack(out);
    }
    else if (strcmp(command, "not") == 0) {
        fprintf(out,
            "@SP\n"
            "A=M-1\n"
            "M=!M\n"
        );
    }
    else if (strcmp(command, "eq") == 0 || strcmp(command, "gt") == 0 || strcmp(command, "lt") == 0){
        char trueLabel[64], endLabel[64];
        sprintf(trueLabel, "TRUE_%d", cw->labelCounter);
        sprintf(endLabel, "END_%d", cw->labelCounter);
        cw->labelCounter++;

        popStackToD(out);   //D = y

        // pop x → M
        fprintf(out,
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M-D\n"       //D = x-y
        );

        //jump logic
        if (strcmp(command, "eq") == 0)
            fprintf(out, "@%s\nD;JEQ\n", trueLabel);
        else if (strcmp(command, "gt") == 0)
            fprintf(out, "@%s\nD;JGT\n", trueLabel);
        else if (strcmp(command, "lt") == 0)
            fprintf(out, "@%s\nD;JLT\n", trueLabel);

        fprintf(out,    
            "D=0\n"         //if false, set to 0 and jump over the true label
            "@%s\n"
            "0;JMP\n"
            "(%s)\n"        //true label
            "D=-1\n"        //set to -1 as true, 0xFFFF
            "(%s)\n",       //end label
            endLabel, trueLabel, endLabel
        );

        pushDToStack(out);
    }
}

void pushSegment(FILE* out, const char* segment, int value, const char* filename){
    if (strcmp(segment, "constant") == 0) {
        //write the value directly
        fprintf(out,
            "@%d\n"
            "D=A\n", 
            value);
    }
    else if (strcmp(segment, "temp") == 0) {
        //offset by 5 because temp 0 = RAM[5]
        fprintf(out,
            "@%d\n"
            "D=M\n", 
            5 + value);
    }
    else if (strcmp(segment, "pointer") == 0) {
        //pointer segment only has 2 elements, 0 == THIS and 1 == THAT
        fprintf(out,
            "@%s\n"
            "D=M\n", 
            value == 0 ? "THIS" : "THAT");
    }
    else if (strcmp(segment, "static") == 0) {
        //static variables are name fileName.value
        fprintf(out,
            "@%s.%d\n"
            "D=M\n", 
            filename, value);
    }
    else {
        //R13 holds address of base + index
        const char* base = segmentBase(segment);
        computeSegmentAddress(out, base, value);
        fprintf(out, "@R13\n"
            "A=M\n"
            "D=M\n");
    }
    pushDToStack(out);
}

void popSegment(FILE* out, const char* segment, int value, const char* filename) {
    popStackToD(out);
    if (strcmp(segment, "temp") == 0) {
        //starts at temp0 = RAM[5]
        fprintf(out,
            "@%d\n"
            "M=D\n",
            5 + value);
    }
    else if (strcmp(segment, "pointer") == 0) {
        //return in THIS/THAT depending on value
        fprintf(out,
            "@%s\n"
            "M=D\n",
            value == 0 ? "THIS" : "THAT");
    }
    else if (strcmp(segment, "static") == 0) {
        //return M[filename.index]
        fprintf(out,
            "@%s.%d\n"
            "M=D\n",
            filename, value);
    }
    else {
        //compute address then return M[R14]
        const char* base = segmentBase(segment);
        computeSegmentAddress(out, base, value);
        fprintf(out,
            "@R13\n"
            "A=M\n"
            "M=D\n");
    }
}

void codewriter_writePushPop(CodeWriter* cw, int commandType, const char* segment, int value) {
    //0 for push, 1 for pop
    if (commandType == 0)
        pushSegment(cw->out, segment, value, cw->fileName);
    else if (commandType == 1)
        popSegment(cw->out, segment, value, cw->fileName);
}

// Close output file
void codewriter_close(CodeWriter* cw) {
    fclose(cw->out);
}
