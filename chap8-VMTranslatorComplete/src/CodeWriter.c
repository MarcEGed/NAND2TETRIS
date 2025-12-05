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

void codewriter_writeInit(CodeWriter* cw){
    fprintf(cw->out,
        "// Bootstrap code\n"
        "@256\n"       // SP = 256
        "D=A\n"
        "@SP\n"
        "M=D\n"

        "@300\n"       // LCL = 300
        "D=A\n"
        "@LCL\n"
        "M=D\n"

        "@400\n"       // ARG = 400
        "D=A\n"
        "@ARG\n"
        "M=D\n"

        "@3000\n"      // THIS = 3000
        "D=A\n"
        "@THIS\n"
        "M=D\n"

        "@3010\n"      // THAT = 3010
        "D=A\n"
        "@THAT\n"
        "M=D\n"
    );
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
        fprintf(cw->out,"//add\n");
        fprintf(out,
            "@SP\n"
            "M=M-1\n"   // SP--
            "A=M\n"
            "D=M\n"     // D = y (top of stack)
            "A=A-1\n"   // move to x
            "M=D+M\n"); // x = x + y
    }
    else if (strcmp(command, "sub") == 0){
        fprintf(cw->out,"//sub\n");
        fprintf(out,
            "@SP\n"
            "M=M-1\n"   // SP--
            "A=M\n"
            "D=M\n"     // D = y (top of stack)
            "A=A-1\n"   // move to x
            "M=M-D\n"); // x = x - y
    }
    else if (strcmp(command, "neg") == 0){
        fprintf(cw->out,"//neg\n");
        fprintf(out,
            "@SP\n"
            "A=M-1\n"
            "M=-M\n"
        );
    }
    else if (strcmp(command, "and") == 0){
        fprintf(cw->out,"//and\n");
        fprintf(out,
            "@SP\n"
            "M=M-1\n"   // SP--
            "A=M\n"
            "D=M\n"     // D = y (top of stack)
            "A=A-1\n"   // move to x
            "M=D&M\n"   // x = x & y
        );
    }
    else if (strcmp(command, "or") == 0) {
        fprintf(cw->out,"//or\n");
        fprintf(out,
            "@SP\n"
            "M=M-1\n"   // SP--
            "A=M\n"
            "D=M\n"     // D = y (top of stack)
            "A=A-1\n"   // move to x
            "M=D|M\n"   // x = x | y
        );
    }
    else if (strcmp(command, "not") == 0) {
        fprintf(cw->out,"//not\n");
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
            "AM=M-1\n"
            "D=M-D\n"       //D = x-y
        );

        //jump logic
        if (strcmp(command, "eq") == 0){
            fprintf(cw->out,"//JEQ\n");
            fprintf(out, "@%s\nD;JEQ\n", trueLabel);
        }else if (strcmp(command, "gt") == 0){
            fprintf(cw->out,"//JGT\n");
            fprintf(out, "@%s\nD;JGT\n", trueLabel);
        }else if (strcmp(command, "lt") == 0){
            fprintf(cw->out,"//JLT\n");
            fprintf(out, "@%s\nD;JLT\n", trueLabel);
        }

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

void codewriter_writeLabel(CodeWriter* cw, char* label){
    char completeLabel[512];

    if (cw->currentFunction[0] != '\0'){
        snprintf(completeLabel, sizeof(completeLabel), "%s$%s", cw->currentFunction, label);
    }else{
        snprintf(completeLabel, sizeof(completeLabel), "%s", label);
    }

    fprintf(cw->out, "// label %s\n", completeLabel); //generate comment
    fprintf(cw->out, "(%s)\n", completeLabel);        //generate assembly label

}

void codewriter_writeGoto(CodeWriter* cw, char* label){
    char completeLabel[512];

    if (cw->currentFunction[0] != '\0'){
        snprintf(completeLabel, sizeof(completeLabel), "%s$%s", cw->currentFunction, label);
    }else{
        snprintf(completeLabel, sizeof(completeLabel), "%s", label);
    }

    fprintf(cw->out, "// goto %s\n", completeLabel);
    fprintf(cw->out, "@%s\n", completeLabel);
    fprintf(cw->out, "0;JMP");
}

void codewriter_writeIf(CodeWriter* cw, char* label){
    char completeLabel[512];

    if (cw->currentFunction[0] != '\0'){
        snprintf(completeLabel, sizeof(completeLabel), "%s$%s", cw->currentFunction, label);
    }else{
        snprintf(completeLabel, sizeof(completeLabel), "%s", label);
    }

    fprintf(cw->out, "//if-goto %s\n", completeLabel);
    fprintf(cw->out, "@SP\n");                          //A is the address of Stack Pointer
    fprintf(cw->out, "M=M-1\n");                        //Decrement SP to point to the topmost element
    fprintf(cw->out, "A=M\n");                          //A = address of popped value
    fprintf(cw->out, "D=M\n");                          //D = value on top of the stack
    fprintf(cw->out, "@%s\n", completeLabel);           //set A as address of target label
    fprintf(cw->out, "D;JNE\n");                        //if D != 0, jump to target label
}

void codewriter_writeFunction(CodeWriter* cw, const char* functionName, int numLocals){
    //set current function for scoping
    strcpy(cw->currentFunction, functionName);  

    //write function label
    fprintf(cw->out, "// function %s %d\n", functionName, numLocals);
    fprintf(cw->out, "(%s)\n", functionName);

    //set local variables to 0
    for (int i = 0; i < numLocals; i++) {
        fprintf(cw->out,
            "// initialize local %d\n"
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "@SP\n"
            "M=M+1\n", i);
    }
}

void codewriter_writeCall(CodeWriter* cw, const char* functionName, int nArgs){
    //LCL       -> points to the base of the current function
    //ARG       -> points to the arguments passed to the function
    //THIS/THAT -> pointer for internal use
    //THAT is used for array addressing, THIS is a pointer to an object's fields
    char returnLabel[128];

    // if currentFunction is empty, use GLOBAL
    const char* base = (cw->currentFunction[0] ? cw->currentFunction : "GLOBAL");
    sprintf(returnLabel, "%s$ret.%d", base, cw->labelCounter++);

    FILE* out = cw->out;

    fprintf(out, "// call %s %d\n", functionName, nArgs);

    // push return address
    fprintf(out,
        "@%s\n"
        "D=A\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n",
        returnLabel);

    // push LCL
    fprintf(out,
        "@LCL\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n");

    // push ARG
    fprintf(out,
        "@ARG\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n");

    // push THIS
    fprintf(out,
        "@THIS\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n");

    // push THAT
    fprintf(out,
        "@THAT\n"
        "D=M\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n");

    // ARG = SP - nArgs - 5
    fprintf(out,
        "@SP\n"
        "D=M\n"
        "@%d\n"
        "D=D-A\n"
        "@ARG\n"
        "M=D\n",
        nArgs + 5);

    // LCL = SP
    fprintf(out,
        "@SP\n"
        "D=M\n"
        "@LCL\n"
        "M=D\n");

    // jump to the function
    fprintf(out,
        "@%s\n"
        "0;JMP\n",
        functionName);

    // place return label
    fprintf(out, "(%s)\n", returnLabel);
}

void codewriter_writeReturn(CodeWriter* cw){
    //FRAME = LCL     temp var
    fprintf(cw->out,
        "// return\n"
        "@LCL\n"
        "D=M\n"
        "@R13\n"      //R13 = FRAME
        "M=D\n");

    //RET = *(FRAME - 5)   saved return address
    fprintf(cw->out,
        "@5\n"
        "A=D-A\n"
        "D=M\n"
        "@R14\n"      //R14 = RET
        "M=D\n");

    //ARG = pop()  reposition return value for caller
    fprintf(cw->out,
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@ARG\n"
        "A=M\n"
        "M=D\n");

    //SP = ARG + 1  restore SP of caller
    fprintf(cw->out,
        "@ARG\n"
        "D=M+1\n"
        "@SP\n"
        "M=D\n");

    //THAT = *(FRAME - 1)  restore THAT
    fprintf(cw->out,
        "@R13\n"
        "AM=M-1\n"
        "D=M\n"
        "@THAT\n"
        "M=D\n");

    //THIS = *(FRAME - 2)  restore THIS
    fprintf(cw->out,
        "@R13\n"
        "AM=M-1\n"
        "D=M\n"
        "@THIS\n"
        "M=D\n");
    
    //ARG = *(FRAME - 3)  restore ARG
    fprintf(cw->out,
        "@R13\n"
        "AM=M-1\n"
        "D=M\n"
        "@ARG\n"
        "M=D\n");

    //LCL = *(FRAME - 4)  restore LCL
    fprintf(cw->out,
        "@R13\n"
        "AM=M-1\n"
        "D=M\n"
        "@LCL\n"
        "M=D\n");

    //goto RET
    fprintf(cw->out,
        "@R14\n"
        "A=M\n"
        "0;JMP\n");
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
    if (strcmp(segment, "temp") == 0) {
        popStackToD(out);
        //starts at temp0 = RAM[5]
        fprintf(out,
            "@%d\n"
            "M=D\n",
            5 + value);
    }
    else if (strcmp(segment, "pointer") == 0) {
        popStackToD(out);
        //return in THIS/THAT depending on value
        fprintf(out,
            "@%s\n"
            "M=D\n",
            value == 0 ? "THIS" : "THAT");
    }
    else if (strcmp(segment, "static") == 0) {
        popStackToD(out);
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
        popStackToD(out);

        fprintf(out,
            "@R13\n"
            "A=M\n"
            "M=D\n");
    }
}

void codewriter_writePushPop(CodeWriter* cw, int commandType, const char* segment, int value) {
    //0 for push, 1 for pop
    if (commandType == 0){
        fprintf(cw->out,"//push %d\n", value);
        pushSegment(cw->out, segment, value, cw->fileName);
    }else if (commandType == 1){
        fprintf(cw->out,"//pop %d\n", value);
        popSegment(cw->out, segment, value, cw->fileName);
    }
}

// Close output file
void codewriter_close(CodeWriter* cw) {
    fclose(cw->out);
}
