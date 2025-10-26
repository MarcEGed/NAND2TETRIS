#ifndef INSTRUCTION_T_H
#define INSTRUCTION_T_H

typedef enum { A_INSTRUCTION, C_INSTRUCTION, L_INSTRUCTION} InstructionType;

typedef struct {
    InstructionType type;
    char dest[5];
    char comp[10];
    char jump[5];
    char symbol[20]; 
    int value;
} Instruction;

#endif